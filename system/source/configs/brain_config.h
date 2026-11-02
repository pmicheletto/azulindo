#ifndef AZULINDO_CONFIGS_BRAIN_CONFIG_H_
#define AZULINDO_CONFIGS_BRAIN_CONFIG_H_

#include <cstdint>

namespace BrainConfig {

inline constexpr const char *kDefaultGgufFilename =
    "Qwen3-1.7B-Q4_K_M.gguf";
    //"Qwen3-4B-Q4_K_M.gguf";
inline constexpr const char *kModelEnvVar = "AZULINDO_MODEL";

inline constexpr bool kDebugLogging = true;

// Smaller KV cache = less RAM pressure and faster prompt eval on CPU.
inline constexpr int32_t kContextTokens = 1024;
// Leave spare cores for UI/compositor; 4 is a good CPU-only tradeoff on 8 cores.
inline constexpr int32_t kDecodeThreads = 4;
inline constexpr int32_t kBatchThreads = 4;
inline constexpr int kMaxGeneratedTokens = 80;
inline constexpr int kTokenPieceBufBytes = 128;

// Silent boot inference: caches the system KV and warms decode/sampler so the
// first user prompt feels snappier (lower time-to-first-token).
inline constexpr bool kWarmupOnBoot = true;
inline constexpr int kWarmupMaxTokens = 8;
inline constexpr const char *kWarmupUserMessage = "hi";

inline constexpr int32_t kPenaltyLastN = 256;
inline constexpr float kRepeatPenalty = 1.18f;
// Seeded with prompt history each turn (see Brain::ProcessTask).
inline constexpr float kFrequencyPenalty = 0.20f;
inline constexpr float kPresencePenalty = 0.35f;

inline constexpr float kSampleTemperature = 1.05f;
inline constexpr float kTopP = 0.92f;
inline constexpr int32_t kTopK = 100;

}  // namespace BrainConfig

#endif  // AZULINDO_CONFIGS_BRAIN_CONFIG_H_
