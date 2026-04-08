#include "brain.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

#include "configs/brain_config.h"
#include "configs/prompt_config.h"

namespace fs = std::filesystem;

namespace {

constexpr int kTokenPieceBufBytes = 128;

std::string ResolveModelPath() {
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

void StripAtMarker(std::string *accum, const char *marker) {
  const size_t hit = accum->find(marker);
  if (hit != std::string::npos) {
    accum->resize(hit);
  }
}

}  // namespace

Brain::Brain() : running_(true) {
  if (InitializeLlama()) {
    worker_thread_ = std::thread(&Brain::WorkerLoop, this);
  }
}

Brain::~Brain() {
  running_ = false;
  queue_cv_.notify_one();
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }
  if (ctx) {
    llama_free(ctx);
  }
  if (model) {
    llama_free_model(model);
  }
  llama_backend_free();
}

bool Brain::InitializeLlama() {
  llama_backend_init();
  const std::string path = ResolveModelPath();

  llama_model_params m_params = llama_model_default_params();
  model = llama_load_model_from_file(path.c_str(), m_params);
  if (!model) {
    return false;
  }

  llama_context_params c_params = llama_context_default_params();
  c_params.n_ctx = BrainConfig::kContextTokens;
  c_params.n_threads = BrainConfig::kDecodeThreads;

  ctx = llama_new_context_with_model(model, c_params);
  return ctx != nullptr;
}

bool Brain::RequestInference(const std::string &input) {
  if (!LlamaReady() || worker_busy_) {
    return false;
  }
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
  std::string t = token_buffer_.front();
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
      worker_busy_ = true;
    }
    ProcessTask(current_input);
    worker_busy_ = false;
  }
}

void Brain::ProcessTask(const std::string &input) {
  if (!model || !ctx) {
    return;
  }

  std::cout << "\n[DEBUG] Prompt received, tokenizing..." << std::endl;

  const llama_vocab *vocab = llama_model_get_vocab(model);

  llama_kv_self_clear(ctx);

  std::vector<llama_token> tokens(input.size() + 32);
  int32_t n_tokens = llama_tokenize(
      vocab, input.c_str(), static_cast<int32_t>(input.size()), tokens.data(),
      static_cast<int32_t>(tokens.size()), true, true);
  tokens.resize(static_cast<size_t>(n_tokens));

  llama_batch batch = llama_batch_get_one(tokens.data(), n_tokens);
  if (llama_decode(ctx, batch) != 0) {
    std::cerr << "[DEBUG] Prompt decode failed." << std::endl;
    return;
  }

  std::cout << "[DEBUG] Prompt processed, generating..." << std::endl;

  llama_sampler *chain =
      llama_sampler_chain_init(llama_sampler_chain_default_params());
  llama_sampler_chain_add(
      chain, llama_sampler_init_penalties(BrainConfig::kPenaltyLastN,
                                         BrainConfig::kRepeatPenalty, 0.0f,
                                         0.0f));
  llama_sampler_chain_add(chain,
                          llama_sampler_init_top_p(BrainConfig::kTopP, 1));
  llama_sampler_chain_add(
      chain, llama_sampler_init_temp(BrainConfig::kSampleTemperature));
  llama_sampler_chain_add(chain, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));

  std::string gen_accum;
  size_t emitted = 0;

  for (int i = 0; i < BrainConfig::kMaxGeneratedTokens; i++) {
    if (!running_) {
      break;
    }

    llama_token next_token = llama_sampler_sample(chain, ctx, -1);

    if (llama_vocab_is_eog(vocab, next_token)) {
      std::cout << "\n[DEBUG] EOS." << std::endl;
      break;
    }

    char buf[kTokenPieceBufBytes];
    int32_t n =
        llama_token_to_piece(vocab, next_token, buf, sizeof(buf), 0, true);

    if (n > 0) {
      std::string piece(buf, static_cast<size_t>(n));
      std::cout << piece << std::flush;
      gen_accum += piece;
      const size_t before_strip = gen_accum.size();
      StripAtMarker(&gen_accum, PromptConfig::kImStartMarker);
      StripAtMarker(&gen_accum, PromptConfig::kImEndMarker);
      const bool cut = gen_accum.size() < before_strip;
      if (gen_accum.size() > emitted) {
        std::string delta = gen_accum.substr(emitted);
        emitted = gen_accum.size();
        if (!delta.empty()) {
          std::lock_guard<std::mutex> lock(buffer_mutex_);
          token_buffer_.push(std::move(delta));
        }
      }
      if (cut) {
        break;
      }
    }

    llama_batch next_batch = llama_batch_get_one(&next_token, 1);
    if (llama_decode(ctx, next_batch) != 0) {
      std::cerr << "\n[DEBUG] Generated token decode failed." << std::endl;
      break;
    }
  }

  std::cout << "\n[DEBUG] Generation finished." << std::endl;
  llama_sampler_free(chain);
}