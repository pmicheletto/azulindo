#include "brain.h"
#include <cstdlib>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace {
    constexpr const char* kDefaultGguf = "tinyllama-1.1b-chat-v1.0.Q2_K.gguf";

    std::string ResolveModelPath() {
        if (const char* env = std::getenv("AZULINDO_MODEL")) {
            if (env[0] != '\0') return std::string(env);
        }
        const char* dirs[] = {"../../models/", "models/", "../models/", "../../../models/"};
        for (const char* dir : dirs) {
            fs::path p = fs::path(dir) / kDefaultGguf;
            if (fs::is_regular_file(p)) return p.lexically_normal().string();
        }
        return "models/" + std::string(kDefaultGguf);
    }
}

Brain::Brain() : running_(true) {
    if (InitializeLlama()) {
        worker_thread_ = std::thread(&Brain::Run, this);
    }
}

Brain::~Brain() {
    running_ = false;
    cv_.notify_one();
    if (worker_thread_.joinable()) worker_thread_.join();
    if (ctx) llama_free(ctx);
    if (model) llama_free_model(model);
    llama_backend_free();
}

bool Brain::InitializeLlama() {
    llama_backend_init();
    const std::string path = ResolveModelPath();
    
    llama_model_params m_params = llama_model_default_params();
    model = llama_load_model_from_file(path.c_str(), m_params);
    if (!model) return false;

    llama_context_params c_params = llama_context_default_params();
    c_params.n_ctx = 2048;
    c_params.n_threads = 2; 
    
    ctx = llama_new_context_with_model(model, c_params);
    return ctx != nullptr;
}

bool Brain::RequestInference(const std::string& input) {
    if (!LlamaReady() || worker_busy_) return false;
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        while(!task_queue_.empty()) task_queue_.pop(); // Limpa fila anterior
        task_queue_.push(input);
    }
    cv_.notify_one();
    return true;
}

std::string Brain::PopToken() {
    std::lock_guard<std::mutex> lock(buffer_mutex_);
    if (token_buffer_.empty()) return "";
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

void Brain::Run() {
    while (running_) {
        std::string current_input;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            cv_.wait(lock, [this] { return !task_queue_.empty() || !running_; });
            if (!running_) break;
            current_input = std::move(task_queue_.front());
            task_queue_.pop();
            worker_busy_ = true;
        }
        ProcessTask(current_input);
        worker_busy_ = false;
    }
}

void Brain::ProcessTask(const std::string& input) {
    if (!model || !ctx) return;
    
    std::cout << "\n[DEBUG] IA Recebeu prompt. Tokenizando..." << std::endl;

    const llama_vocab* vocab = llama_model_get_vocab(model);
    
    llama_kv_self_clear(ctx);

    std::vector<llama_token> tokens(input.size() + 32);
    int32_t n_tokens = llama_tokenize(vocab, input.c_str(), (int32_t)input.size(), 
                                      tokens.data(), (int32_t)tokens.size(), true, true);
    tokens.resize(n_tokens);

    llama_batch batch = llama_batch_get_one(tokens.data(), n_tokens);
    if (llama_decode(ctx, batch) != 0) {
        std::cerr << "[DEBUG] Erro no decode do prompt!" << std::endl;
        return;
    }

    std::cout << "[DEBUG] Prompt processado. Gerando resposta..." << std::endl;

    struct llama_sampler* smpl = llama_sampler_init_greedy();
    
    int32_t n_cur = n_tokens; 

    for (int i = 0; i < 128; i++) {
        if (!running_) break;

        llama_token next_token = llama_sampler_sample(smpl, ctx, -1); 

        if (llama_vocab_is_eog(vocab, next_token)) {
            std::cout << "\n[DEBUG] EOS detectado." << std::endl;
            break;
        }

        char buf[128];
        int32_t n = llama_token_to_piece(vocab, next_token, buf, sizeof(buf), 0, true);
        
        if (n > 0) {
            std::string piece(buf, n);
            
            std::cout << piece << std::flush; 
            
            // Manda para a Raylib
            std::lock_guard<std::mutex> lock(buffer_mutex_);
            token_buffer_.push(piece);
        }

        llama_batch next_batch = llama_batch_get_one(&next_token, 1);
        
        if (llama_decode(ctx, next_batch) != 0) {
            std::cerr << "\n[DEBUG] Erro no decode do token gerado!" << std::endl;
            break;
        }
        
        n_cur++;
    }
    
    std::cout << "\n[DEBUG] Fim da geração." << std::endl;
    llama_sampler_free(smpl);
}