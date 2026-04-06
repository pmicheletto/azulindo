#ifndef BRAIN_H
#define BRAIN_H

#include "llama.h"
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

class Brain {
public:
    Brain();
    ~Brain();

    bool RequestInference(const std::string& input);
    std::string PopToken(); // Sistema de "pesca" de tokens para a Raylib
    bool LlamaReady() const { return model && ctx; }
    bool IsStreamingTokens() const;

private:
    bool InitializeLlama();
    void Run();
    void ProcessTask(const std::string& input);

    llama_model* model = nullptr;
    llama_context* ctx = nullptr;

    std::thread worker_thread_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    std::queue<std::string> task_queue_;
    
    std::atomic<bool> running_;
    std::atomic<bool> worker_busy_{false};

    // Buffer seguro para a Raylib ler
    std::queue<std::string> token_buffer_;
    mutable std::mutex buffer_mutex_;
};

#endif