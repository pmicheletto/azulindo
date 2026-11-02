#include "core/brain/brain.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "configs/brain_config.h"
#include "configs/prompt_config.h"

namespace fs = std::filesystem;

namespace {

#define AZULINDO_BRAIN_DEBUG(expr)          \
  do {                                      \
    if constexpr (BrainConfig::kDebugLogging) { \
      expr;                                 \
    }                                       \
  } while (0)

std::string ResolveModelPath(const std::string &override_path) {
  if (!override_path.empty()) {
    return override_path;
  }
  if (const char *env = std::getenv(BrainConfig::kModelEnvVar)) {
    if (env[0] != '\0') {
      return std::string(env);
    }
  }
  const char *dirs[] = {"../../models/", "models/", "../models/",
                        "../../../models/"};
  for (const char *dir : dirs) {
    fs::path p = fs::path(dir) / BrainConfig::kDefaultGgufFilename;
    if (fs::is_regular_file(p)) {
      return p.lexically_normal().string();
    }
  }
  return std::string("models/") + BrainConfig::kDefaultGgufFilename;
}

std::vector<llama_token> Tokenize(const llama_vocab *vocab, const char *text,
                                  bool add_special, bool parse_special) {
  const int32_t text_len =
      static_cast<int32_t>(std::char_traits<char>::length(text));
  const int32_t n = -llama_tokenize(vocab, text, text_len, nullptr, 0,
                                    add_special, parse_special);
  if (n <= 0) {
    return {};
  }
  std::vector<llama_token> tokens(static_cast<size_t>(n));
  const int32_t written =
      llama_tokenize(vocab, text, text_len, tokens.data(), n, add_special,
                     parse_special);
  if (written != n) {
    return {};
  }
  return tokens;
}

std::unordered_set<llama_token> BuildStopTokens(const llama_vocab *vocab) {
  std::unordered_set<llama_token> stops;
  for (std::size_t i = 0; i < PromptConfig::kGenerationStopStringCount; ++i) {
    const char *stop = PromptConfig::kGenerationStopStrings[i];
    const std::vector<llama_token> toks =
        Tokenize(vocab, stop, /*add_special=*/false, /*parse_special=*/true);
    if (toks.size() == 1) {
      stops.insert(toks[0]);
      continue;
    }
    std::cerr << "[brain] stop string did not tokenize to one special token: "
              << stop << " (got " << toks.size() << ")\n";
    if (!toks.empty()) {
      stops.insert(toks.front());
    }
  }
  return stops;
}

std::vector<llama_logit_bias> BuildContinuationBiases(const llama_vocab *vocab) {
  std::vector<llama_logit_bias> biases;
  const std::vector<llama_token> toks =
      Tokenize(vocab, PromptConfig::kImStartMarker, /*add_special=*/false,
               /*parse_special=*/true);
  biases.reserve(toks.size());
  for (const llama_token token : toks) {
    biases.push_back(llama_logit_bias{token, -INFINITY});
  }
  return biases;
}

std::string BuildSystemPrefixText(const char *personality) {
  // Must stay identical to PromptBuilder::BuildSystemPrefix (chatml).
  std::string out;
  out.reserve(256);
  out += "<|im_start|>system\n";
  out += personality;
  out += "<|im_end|>\n";
  return out;
}

}  // namespace

Brain::Brain(std::string model_path)
    : model_path_(ResolveModelPath(std::move(model_path))),
      personality_(PromptConfig::PersonalityFor(PromptConfig::kDefaultLanguage)),
      running_(true),
      loading_(true),
      ready_(false) {
  // Load the GGUF off the UI thread so the window keeps responding.
  worker_thread_ = std::thread(&Brain::BootstrapAndRun, this);
}

std::string Brain::ModelDisplayName() const {
  return fs::path(model_path_).stem().string();
}

void Brain::SetPersonality(const char *personality) {
  if (personality == nullptr || personality[0] == '\0') {
    return;
  }
  std::lock_guard<std::mutex> lock(personality_mutex_);
  if (personality_ == personality) {
    return;
  }
  personality_ = personality;
  personality_dirty_.store(true, std::memory_order_release);
}

bool Brain::RebuildSystemPrefixIfNeeded() {
  if (!personality_dirty_.load(std::memory_order_acquire)) {
    return true;
  }
  if (!model_) {
    return false;
  }

  std::string personality;
  {
    std::lock_guard<std::mutex> lock(personality_mutex_);
    personality = personality_;
  }

  system_prefix_ = BuildSystemPrefixText(personality.c_str());
  const llama_vocab *vocab = llama_model_get_vocab(model_);
  system_tokens_ =
      Tokenize(vocab, system_prefix_.c_str(), /*add_special=*/false,
               /*parse_special=*/true);
  system_n_ = static_cast<int32_t>(system_tokens_.size());
  system_cached_ = false;
  personality_dirty_.store(false, std::memory_order_release);

  if (system_n_ <= 0) {
    std::cerr << "[brain] failed to tokenize system prefix after language change\n";
    return false;
  }
  AZULINDO_BRAIN_DEBUG(std::cout << "[DEBUG] System prefix rebuilt ("
                                 << system_n_ << " tokens)\n");
  return true;
}

Brain::~Brain() {
  running_ = false;
  queue_cv_.notify_one();
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }
  if (ctx_) {
    llama_free(ctx_);
    ctx_ = nullptr;
  }
  if (model_) {
    llama_model_free(model_);
    model_ = nullptr;
  }
  if (backend_inited_) {
    llama_backend_free();
    backend_inited_ = false;
  }
}

bool Brain::InitializeLlama() {
  llama_backend_init();
  backend_inited_ = true;

  std::cerr << "[brain] loading model: " << model_path_ << '\n';

  llama_model_params m_params = llama_model_default_params();
  model_ = llama_model_load_from_file(model_path_.c_str(), m_params);
  if (!model_) {
    std::cerr << "[brain] failed to load model file\n";
    return false;
  }

  llama_context_params c_params = llama_context_default_params();
  c_params.n_ctx = BrainConfig::kContextTokens;
  c_params.n_threads = BrainConfig::kDecodeThreads;
  c_params.n_threads_batch = BrainConfig::kBatchThreads;

  ctx_ = llama_init_from_model(model_, c_params);
  if (!ctx_) {
    std::cerr << "[brain] failed to create llama context\n";
    llama_model_free(model_);
    model_ = nullptr;
    return false;
  }

  const llama_vocab *vocab = llama_model_get_vocab(model_);
  stop_tokens_ = BuildStopTokens(vocab);
  continuation_biases_ = BuildContinuationBiases(vocab);
  {
    std::lock_guard<std::mutex> lock(personality_mutex_);
    system_prefix_ = BuildSystemPrefixText(personality_.c_str());
  }
  system_tokens_ =
      Tokenize(vocab, system_prefix_.c_str(), /*add_special=*/false,
               /*parse_special=*/true);
  system_n_ = static_cast<int32_t>(system_tokens_.size());
  if (system_n_ <= 0) {
    std::cerr << "[brain] failed to tokenize system prefix\n";
    return false;
  }
  personality_dirty_.store(false, std::memory_order_release);
  return true;
}

bool Brain::EnsureSystemPrefixCached() {
  if (system_cached_) {
    return true;
  }
  llama_memory_t mem = llama_get_memory(ctx_);
  llama_memory_clear(mem, /*data=*/true);
  llama_batch batch =
      llama_batch_get_one(system_tokens_.data(), system_n_);
  if (llama_decode(ctx_, batch) != 0) {
    std::cerr << "[brain] system prefix decode failed\n";
    return false;
  }
  system_cached_ = true;
  AZULINDO_BRAIN_DEBUG(std::cout << "[DEBUG] System prefix cached ("
                                 << system_n_ << " tokens)\n");
  return true;
}

void Brain::BootstrapAndRun() {
  const bool ok = InitializeLlama();
  if (!ok || !running_.load(std::memory_order_acquire)) {
    ready_.store(false, std::memory_order_release);
    loading_.store(false, std::memory_order_release);
    running_.store(false, std::memory_order_release);
    return;
  }
  // Warm before advertising ready so the first interactive turn is fast and
  // no warmup tokens leak into the HUD.
  if (BrainConfig::kWarmupOnBoot) {
    RunWarmup();
  }
  if (!running_.load(std::memory_order_acquire)) {
    ready_.store(false, std::memory_order_release);
    loading_.store(false, std::memory_order_release);
    return;
  }
  ready_.store(true, std::memory_order_release);
  loading_.store(false, std::memory_order_release);
  WorkerLoop();
}

void Brain::RunWarmup() {
  // Same ChatML shape as PromptBuilder::BuildChatml (empty history).
  std::string prompt;
  prompt.reserve(system_prefix_.size() + 64);
  prompt += system_prefix_;
  prompt += "<|im_start|>user\n";
  prompt += BrainConfig::kWarmupUserMessage;
  prompt += "<|im_end|>\n<|im_start|>assistant\n";
  prompt += PromptConfig::kDisableThinkingPrefix;

  AZULINDO_BRAIN_DEBUG(std::cout << "[DEBUG] Silent warmup inference...\n");
  ProcessTask(prompt, /*silent=*/true, BrainConfig::kWarmupMaxTokens);
  FlushTokenBuffer();
  AZULINDO_BRAIN_DEBUG(std::cout << "[DEBUG] Warmup done (system KV cached).\n");
}

void Brain::FlushTokenBuffer() {
  std::lock_guard<std::mutex> lock(buffer_mutex_);
  while (!token_buffer_.empty()) {
    token_buffer_.pop();
  }
}

bool Brain::RequestInference(const std::string &input) {
  if (!IsReady() || worker_busy_.load(std::memory_order_acquire)) {
    return false;
  }
  FlushTokenBuffer();
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    while (!task_queue_.empty()) {
      task_queue_.pop();
    }
    task_queue_.push(input);
  }
  queue_cv_.notify_one();
  return true;
}

std::string Brain::PopToken() {
  std::lock_guard<std::mutex> lock(buffer_mutex_);
  if (token_buffer_.empty()) {
    return "";
  }
  std::string t = std::move(token_buffer_.front());
  token_buffer_.pop();
  return t;
}

bool Brain::IsStreamingTokens() const {
  if (worker_busy_.load(std::memory_order_acquire)) {
    return true;
  }
  std::lock_guard<std::mutex> lock(buffer_mutex_);
  return !token_buffer_.empty();
}

void Brain::WorkerLoop() {
  while (running_) {
    std::string current_input;
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      queue_cv_.wait(lock,
                     [this] { return !task_queue_.empty() || !running_; });
      if (!running_) {
        break;
      }
      current_input = std::move(task_queue_.front());
      task_queue_.pop();
      worker_busy_.store(true, std::memory_order_release);
    }
    ProcessTask(current_input);
    worker_busy_.store(false, std::memory_order_release);
  }
}

void Brain::ProcessTask(const std::string &input, bool silent,
                        int max_tokens) {
  if (!model_ || !ctx_) {
    return;
  }
  if (!RebuildSystemPrefixIfNeeded()) {
    return;
  }

  const int token_limit =
      max_tokens < 0 ? BrainConfig::kMaxGeneratedTokens : max_tokens;

  if (!silent) {
    AZULINDO_BRAIN_DEBUG(std::cout << "\n===== PROMPT =====\n"
                                   << input
                                   << "\n==================\n");
  }

  const llama_vocab *vocab = llama_model_get_vocab(model_);
  llama_memory_t mem = llama_get_memory(ctx_);

  std::vector<llama_token> prompt_tokens;
  const bool use_prefix_cache =
      !system_prefix_.empty() &&
      input.size() >= system_prefix_.size() &&
      input.compare(0, system_prefix_.size(), system_prefix_) == 0;

  if (use_prefix_cache) {
    if (!EnsureSystemPrefixCached()) {
      return;
    }
    // Drop previous user turn + generation; keep system KV.
    if (!llama_memory_seq_rm(mem, /*seq_id=*/0, /*p0=*/system_n_, /*p1=*/-1)) {
      std::cerr << "[brain] failed to trim KV after system prefix; "
                   "rebuilding cache\n";
      system_cached_ = false;
      if (!EnsureSystemPrefixCached()) {
        return;
      }
    }
    const std::string suffix = input.substr(system_prefix_.size());
    prompt_tokens =
        Tokenize(vocab, suffix.c_str(), /*add_special=*/false,
                 /*parse_special=*/true);
  } else {
    system_cached_ = false;
    llama_memory_clear(mem, /*data=*/true);
    prompt_tokens =
        Tokenize(vocab, input.c_str(), /*add_special=*/false,
                 /*parse_special=*/true);
  }

  if (prompt_tokens.empty()) {
    std::cerr << "[brain] tokenize failed\n";
    return;
  }
  const int32_t n_tokens = static_cast<int32_t>(prompt_tokens.size());

  llama_batch batch = llama_batch_get_one(prompt_tokens.data(), n_tokens);
  if (llama_decode(ctx_, batch) != 0) {
    std::cerr << "[brain] prompt decode failed\n";
    system_cached_ = false;
    return;
  }

  AZULINDO_BRAIN_DEBUG(std::cout << "[DEBUG] Prompt processed, generating..."
                                 << std::endl);

  llama_sampler *chain =
      llama_sampler_chain_init(llama_sampler_chain_default_params());
  // Sampler order: penalties -> top_k -> top_p -> temperature -> dist
  llama_sampler_chain_add(
      chain, llama_sampler_init_penalties(BrainConfig::kPenaltyLastN,
                                         BrainConfig::kRepeatPenalty,
                                         BrainConfig::kFrequencyPenalty,
                                         BrainConfig::kPresencePenalty));
  if (!continuation_biases_.empty()) {
    llama_sampler_chain_add(
        chain,
        llama_sampler_init_logit_bias(
            llama_vocab_n_tokens(vocab),
            static_cast<int32_t>(continuation_biases_.size()),
            continuation_biases_.data()));
  }
  llama_sampler_chain_add(chain,
                          llama_sampler_init_top_k(BrainConfig::kTopK));
  llama_sampler_chain_add(chain,
                          llama_sampler_init_top_p(BrainConfig::kTopP, 1));
  llama_sampler_chain_add(
      chain, llama_sampler_init_temp(BrainConfig::kSampleTemperature));
  llama_sampler_chain_add(chain, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

  // Seed penalties with recent prompt tokens so catchphrases from earlier
  // turns (in chat history) are discouraged — a fresh sampler alone only
  // sees tokens accepted during this generation.
  {
    const int32_t n_prompt = static_cast<int32_t>(prompt_tokens.size());
    const int32_t seed_n =
        std::min(n_prompt, BrainConfig::kPenaltyLastN);
    for (int32_t i = n_prompt - seed_n; i < n_prompt; ++i) {
      llama_sampler_accept(chain, prompt_tokens[static_cast<std::size_t>(i)]);
    }
  }

  for (int i = 0; i < token_limit; i++) {
    if (!running_) {
      break;
    }

    llama_token next_token = llama_sampler_sample(chain, ctx_, -1);

    if (llama_vocab_is_eog(vocab, next_token) ||
        stop_tokens_.count(next_token) > 0) {
      AZULINDO_BRAIN_DEBUG(std::cout << "\n[DEBUG] Stop token." << std::endl);
      break;
    }

    char buf[BrainConfig::kTokenPieceBufBytes];
    int32_t n =
        llama_token_to_piece(vocab, next_token, buf, sizeof(buf), 0, false);

    if (n > 0) {
      std::string piece(buf, static_cast<size_t>(n));
      AZULINDO_BRAIN_DEBUG({
        if (!silent) {
          std::cout << piece << std::flush;
        }
      });
      if (!silent) {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        token_buffer_.push(std::move(piece));
      }
    }

    llama_batch next_batch = llama_batch_get_one(&next_token, 1);
    if (llama_decode(ctx_, next_batch) != 0) {
      std::cerr << "[brain] generated token decode failed\n";
      break;
    }
  }

  AZULINDO_BRAIN_DEBUG(std::cout << "\n[DEBUG] Generation finished."
                                 << std::endl);
  llama_sampler_free(chain);
}
