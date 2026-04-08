#ifndef AZULINDO_CORE_BRAIN_BRAIN_H_
#define AZULINDO_CORE_BRAIN_BRAIN_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "llama.h"

class Brain {
 public:
  Brain();
  ~Brain();

  bool RequestInference(const std::string &input);
  std::string PopToken();
  bool LlamaReady() const { return model && ctx; }
  bool IsStreamingTokens() const;

 private:
  bool InitializeLlama();
  void WorkerLoop();
  void ProcessTask(const std::string &input);

  llama_model *model = nullptr;
  llama_context *ctx = nullptr;

  std::thread worker_thread_;
  std::mutex queue_mutex_;
  std::condition_variable queue_cv_;
  std::queue<std::string> task_queue_;

  std::atomic<bool> running_;
  std::atomic<bool> worker_busy_{false};

  std::queue<std::string> token_buffer_;
  mutable std::mutex buffer_mutex_;
};

#endif
