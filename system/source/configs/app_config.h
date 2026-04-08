#ifndef INTERFACE_SOURCE_CONFIGS_APP_CONFIG_H_
#define INTERFACE_SOURCE_CONFIGS_APP_CONFIG_H_

namespace AppConfig {

inline constexpr const char *kWindowTitle = "Azulindo - Ford Ka 2002";
inline constexpr int kTargetFps = 60;
inline constexpr int kDefaultWindowWidth = 800;
inline constexpr int kDefaultWindowHeight = 480;

inline constexpr const char *kHintLlmOffline =
    "LLM offline: verifique o arquivo .gguf";

// Minimum time between showing two tokens in the UI (larger = slower typing).
inline constexpr float kUiTokenIntervalSeconds = 0.12f;

inline constexpr int kMaxPromptInputChars = 512;

inline constexpr int kPromptBarHeight = 36;
inline constexpr int kPromptBarMargin = 8;
inline constexpr int kPromptFontSize = 18;

}  // namespace AppConfig

#endif
