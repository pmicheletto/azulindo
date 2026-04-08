#ifndef INTERFACE_SOURCE_CONFIGS_BRAIN_CONFIG_H_
#define INTERFACE_SOURCE_CONFIGS_BRAIN_CONFIG_H_

#include <cstdint>

namespace BrainConfig {

inline constexpr const char *kDefaultGgufFilename =
    "qwen2.5-1.5b-instruct-q4_k_m.gguf";
inline constexpr const char *kModelEnvVar = "AZULINDO_MODEL";

inline constexpr int32_t kContextTokens = 2048;
inline constexpr int32_t kDecodeThreads = 2;
inline constexpr int kMaxGeneratedTokens = 256;

inline constexpr int32_t kPenaltyLastN = 128;
inline constexpr float kRepeatPenalty = 1.18f;
inline constexpr float kSampleTemperature = 0.75f;
inline constexpr float kTopP = 0.92f;

}  // namespace BrainConfig

#endif
