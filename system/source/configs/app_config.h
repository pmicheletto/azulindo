#ifndef AZULINDO_CONFIGS_APP_CONFIG_H_
#define AZULINDO_CONFIGS_APP_CONFIG_H_

namespace AppConfig {

inline constexpr const char *kWindowTitle = "Azulindo - Ford Ka 2002";
inline constexpr int kTargetFps = 60;
#ifdef AZULINDO_PLATFORM_DRM
inline constexpr int kDefaultWindowWidth = 704;
inline constexpr int kDefaultWindowHeight = 512;
#else
inline constexpr int kDefaultWindowWidth = 800;
inline constexpr int kDefaultWindowHeight = 480;
#endif

inline constexpr const char *kHintLlmOffline =
    "LLM offline: verifique o arquivo .gguf";
inline constexpr const char *kHintBootScene = "Carregando cenario...";

// Larger = slower on-screen typing / more deliberate stream.
inline constexpr float kUiTokenIntervalSeconds = 0.35f;

inline constexpr int kMaxPromptInputChars = 512;

inline constexpr int kPromptBarHeight = 36;
inline constexpr int kPromptBarMargin = 8;
inline constexpr int kPromptBarPaddingX = 8;
inline constexpr int kPromptBarBorderThickness = 2;
inline constexpr int kPromptFontSize = 18;
inline constexpr float kPromptFocusedAlpha = 0.85f;
inline constexpr float kPromptUnfocusedAlpha = 0.6f;
inline constexpr float kPromptBorderUnfocusedAlpha = 0.5f;
inline constexpr float kPromptPlaceholderAlpha = 0.45f;
inline constexpr const char *kPromptPlaceholder = "ask something...";
inline constexpr const char *kPromptCaret = "_";
inline constexpr const char *kPromptPrefixFocused = "> ";
inline constexpr const char *kPromptPrefixIdle = "[TAB] ";
inline constexpr const char *kPromptOfflineSuffix = " (LLM offline)";

inline constexpr int kStatusFpsX = 10;
inline constexpr int kStatusFpsY = 50;
inline constexpr int kStatusHintX = 10;
inline constexpr int kStatusHintY = 32;
inline constexpr int kStatusHintFontSize = 18;

}  // namespace AppConfig

#endif  // AZULINDO_CONFIGS_APP_CONFIG_H_
