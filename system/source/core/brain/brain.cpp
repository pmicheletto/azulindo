#include "core/brain/brain.h"

Brain::Brain()
    : running_(true),
      on_token_generated_(nullptr),
      worker_thread_(&Brain::Run, this) {}

Brain::~Brain() {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        running_ = false;
    }
    cv_.notify_one();

    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void Brain::RequestInference(const std::string& input) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        task_queue_.push(input);
    }
    cv_.notify_one();
}

void Brain::SetOnTokenGenerated(
    std::function<void(const std::string&)> callback) {
    on_token_generated_ = callback;
}

void Brain::Run() {
    while (running_) {
        std::string current_input;

        {
            std::unique_lock<std::mutex> lock(queue_mutex_);

            cv_.wait(lock,
                     [this] { return !task_queue_.empty() || !running_; });

            if (!running_ && task_queue_.empty()) break;

            current_input = std::move(task_queue_.front());
            task_queue_.pop();
        }

        ProcessTask(current_input);
    }
}

void Brain::ProcessTask(const std::string& input) {
    if (on_token_generated_) {
        on_token_generated_("Azulindo processando: " + input);
    }
}