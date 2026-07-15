#include "screen/ai_text_pipeline.h"

#include "configs/layout_config.h"
#include "raylib.h"
#include "screen/dialogue_panel.h"
#include "screen/text_layout.h"

AiTextPipeline::AiTextPipeline(DialoguePanel &dialogue)
    : dialogue_(dialogue),
      worker_(&AiTextPipeline::IngestLoop, this) {}

AiTextPipeline::~AiTextPipeline() {
  {
    std::lock_guard<std::mutex> lock(ingest_mutex_);
    worker_running_ = false;
  }
  ingest_cv_.notify_all();
  if (worker_.joinable()) {
    worker_.join();
  }
}

void AiTextPipeline::Append(const std::string &text) {
  {
    std::lock_guard<std::mutex> lock(ingest_mutex_);
    ingest_queue_.push(text);
  }
  ingest_cv_.notify_one();
}

void AiTextPipeline::Clear() {
  {
    std::lock_guard<std::mutex> lock(ingest_mutex_);
    while (!ingest_queue_.empty()) {
      ingest_queue_.pop();
    }
  }
  {
    std::lock_guard<std::mutex> lock(text_mutex_);
    ai_response_.clear();
  }
}

bool AiTextPipeline::IsBusy() const {
  if (ingest_active_.load(std::memory_order_acquire)) {
    return true;
  }
  std::lock_guard<std::mutex> lock(ingest_mutex_);
  return !ingest_queue_.empty();
}

std::string AiTextPipeline::Snapshot() const {
  std::lock_guard<std::mutex> lock(text_mutex_);
  return ai_response_;
}

void AiTextPipeline::IngestLoop() {
  while (true) {
    std::string chunk;
    {
      std::unique_lock<std::mutex> lock(ingest_mutex_);
      ingest_cv_.wait(lock, [this] {
        return !ingest_queue_.empty() || !worker_running_;
      });
      if (!worker_running_ && ingest_queue_.empty()) {
        break;
      }
      if (ingest_queue_.empty()) {
        continue;
      }
      chunk = std::move(ingest_queue_.front());
      ingest_queue_.pop();
    }

    ingest_active_.store(true, std::memory_order_release);

    const Rectangle area = dialogue_.TextArea();
    const Font font = GetFontDefault();
    const float font_size = LayoutConfig::DialogueConfig::text_font_size;
    const float spacing = LayoutConfig::DialogueConfig::text_letter_spacing;

    {
      std::lock_guard<std::mutex> lock(text_mutex_);
      std::string candidate = ai_response_ + chunk;
      const float wrapped_height = TextLayout::MeasureWrappedContentHeight(
          font, candidate.c_str(), area.width, font_size, spacing);
      if (wrapped_height > area.height) {
        ai_response_ = std::move(chunk);
      } else {
        ai_response_ = std::move(candidate);
      }
    }

    ingest_active_.store(false, std::memory_order_release);
  }
}
