#ifndef AZULINDO_SCREEN_AI_TEXT_PIPELINE_H_
#define AZULINDO_SCREEN_AI_TEXT_PIPELINE_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class DialoguePanel;

class AiTextPipeline {
 public:
  explicit AiTextPipeline(DialoguePanel &dialogue);
  ~AiTextPipeline();

  AiTextPipeline(const AiTextPipeline &) = delete;
  AiTextPipeline &operator=(const AiTextPipeline &) = delete;

  void Append(const std::string &text);
  void Clear();
  bool IsBusy() const;
  std::string Snapshot() const;

 private:
  void IngestLoop();

  DialoguePanel &dialogue_;
  std::string ai_response_ = "AZULINDO: Sistema inicializado...";
  mutable std::mutex text_mutex_;

  std::thread worker_;
  std::atomic<bool> worker_running_{true};
  mutable std::mutex ingest_mutex_;
  std::condition_variable ingest_cv_;
  std::queue<std::string> ingest_queue_;
  std::atomic<bool> ingest_active_{false};
};

#endif  // AZULINDO_SCREEN_AI_TEXT_PIPELINE_H_
