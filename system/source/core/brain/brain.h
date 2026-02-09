#ifndef AZULINDO_CORE_BRAIN_BRAIN_H_
#define AZULINDO_CORE_BRAIN_BRAIN_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class Brain {
 public:
  Brain();
  ~Brain();

  Brain(const Brain&) = delete;
  Brain& operator=(const Brain&) = delete;

  void RequestInference(const std::string& input);

  void SetOnTokenGenerated(std::function<void(const std::string&)> callback);

 private:
  void Run();
  void ProcessTask(const std::string& input);

  std::atomic<bool> running_{true};
  std::mutex queue_mutex_;
  std::condition_variable cv_;
  std::queue<std::string> task_queue_;
  std::function<void(const std::string&)> on_token_generated_ = nullptr;
  
  std::thread worker_thread_;
};

#endif  // AZULINDO_CORE_BRAIN_BRAIN_H_