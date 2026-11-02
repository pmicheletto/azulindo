#ifndef AZULINDO_CORE_BRAIN_BRAIN_H_
#define AZULINDO_CORE_BRAIN_BRAIN_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include "llama.h"

class Brain {
 public:
  explicit Brain(std::string model_path = {});
  ~Brain();

  Brain(const Brain &) = delete;
  Brain &operator=(const Brain &) = delete;

  bool RequestInference(const std::string &input);
  std::string PopToken();
  bool IsReady() const { return ready_.load(std::memory_order_acquire); }
  bool IsLoading() const { return loading_.load(std::memory_order_acquire); }
  bool IsStreamingTokens() const;
  // Filename stem of the resolved GGUF path (e.g. "Qwen3-1.7B-Q4_K_M").
  std::string ModelDisplayName() const;

  // Queues a system-prompt rebuild (applied on the next inference task).
  void SetPersonality(const char *personality);

 private:
  bool InitializeLlama();
  bool EnsureSystemPrefixCached();
  bool RebuildSystemPrefixIfNeeded();
  void FlushTokenBuffer();
  void BootstrapAndRun();
  void RunWarmup();
  void WorkerLoop();
  // silent: generate without pushing pieces into the UI token buffer.
  // max_tokens < 0 uses BrainConfig::kMaxGeneratedTokens.
  void ProcessTask(const std::string &input, bool silent = false,
                   int max_tokens = -1);

  std::string model_path_;
  std::string personality_;
  std::mutex personality_mutex_;
  std::atomic<bool> personality_dirty_{false};

  llama_model *model_ = nullptr;
  llama_context *ctx_ = nullptr;
  bool backend_inited_ = false;

  // System ChatML prefix kept in KV across turns (prompt eval once).
  std::string system_prefix_;
  std::vector<llama_token> system_tokens_;
  int32_t system_n_ = 0;
  bool system_cached_ = false;

  std::unordered_set<llama_token> stop_tokens_;
  std::vector<llama_logit_bias> continuation_biases_;

  std::thread worker_thread_;
  std::mutex queue_mutex_;
  std::condition_variable queue_cv_;
  std::queue<std::string> task_queue_;

  std::atomic<bool> running_{false};
  std::atomic<bool> loading_{true};
  std::atomic<bool> ready_{false};
  std::atomic<bool> worker_busy_{false};

  std::queue<std::string> token_buffer_;
  mutable std::mutex buffer_mutex_;
};

#endif  // AZULINDO_CORE_BRAIN_BRAIN_H_
