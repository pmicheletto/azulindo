#include "app/azulindo_application.h"

#include <cstdint>

#include "configs/app_config.h"
#include "configs/prompt_config.h"
#include "core/brain/brain.h"
#include "core/state_emotion/emotion.h"
#include "raylib.h"
#include "screen/azulindo_screen.h"

namespace {

void AppendCodepointAsUtf8(std::string *s, int codepoint) {
  if (codepoint <= 0) {
    return;
  }
  if (codepoint < 0x80) {
    s->push_back(static_cast<char>(codepoint));
  } else if (codepoint < 0x800) {
    s->push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
    s->push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else if (codepoint < 0x10000) {
    s->push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
    s->push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
    s->push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  }
}

void PopLastUtf8Char(std::string *s) {
  if (s->empty()) {
    return;
  }
  size_t i = s->size() - 1;
  while (i > 0 && (static_cast<std::uint8_t>((*s)[i]) & 0xC0) == 0x80) {
    --i;
  }
  s->erase(i);
}

void ApplyEmotionHotkeys(AzulindoScreen &screen) {
  if (IsKeyPressed(KEY_ONE)) {
    screen.SetEmotion(EmotionState::kIdle);
  } else if (IsKeyPressed(KEY_TWO)) {
    screen.SetEmotion(EmotionState::kAngry);
  } else if (IsKeyPressed(KEY_THREE)) {
    screen.SetEmotion(EmotionState::kThinking);
  }
}

}  // namespace

AzulindoApplication::AzulindoApplication(int initial_width, int initial_height)
    : initial_width_(initial_width), initial_height_(initial_height) {}

void AzulindoApplication::DrainTokensThrottled(Brain &brain,
                                               AzulindoScreen &screen,
                                               float dt) {
  token_display_cooldown_ -= dt;
  if (token_display_cooldown_ > 0.0f) {
    return;
  }
  std::string token = brain.PopToken();
  if (token.empty()) {
    token_display_cooldown_ = 0.0f;
    return;
  }
  screen.AppendAiText(token);
  screen.SetEmotion(EmotionState::kIdle);
  token_display_cooldown_ = AppConfig::kUiTokenIntervalSeconds;
}

void AzulindoApplication::UpdatePromptInput(Brain &brain,
                                            AzulindoScreen &screen) {
  if (IsKeyPressed(KEY_TAB)) {
    prompt_focus_ = !prompt_focus_;
    return;
  }

  if (!prompt_focus_) {
    return;
  }

  if (IsKeyPressed(KEY_ESCAPE)) {
    prompt_focus_ = false;
    prompt_input_.clear();
    return;
  }

  const bool enter = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
  if (enter) {
    if (prompt_input_.empty()) {
      prompt_focus_ = false;
      return;
    }
    if (!brain.LlamaReady()) {
      return;
    }
    std::string full = std::string(PromptConfig::kImStartSystem) +
                       PromptConfig::kPersonality + "\n" +
                       std::string(PromptConfig::kImEndMarker) + "\n" +
                       PromptConfig::kImStartUser + prompt_input_ + "\n" +
                       std::string(PromptConfig::kImEndMarker) + "\n" +
                       PromptConfig::kImStartAssistant;
    if (brain.RequestInference(full)) {
      screen.ClearAiText();
      screen.SetEmotion(EmotionState::kThinking);
      prompt_input_.clear();
      token_display_cooldown_ = 0.0f;
      prompt_focus_ = false;
    }
    return;
  }

  if (IsKeyPressed(KEY_BACKSPACE)) {
    PopLastUtf8Char(&prompt_input_);
  }

  int key = 0;
  while ((key = GetCharPressed()) > 0) {
    if (key == '\n' || key == '\r') {
      continue;
    }
    if (static_cast<size_t>(prompt_input_.size()) >=
        AppConfig::kMaxPromptInputChars) {
      break;
    }
    AppendCodepointAsUtf8(&prompt_input_, key);
  }
}

void AzulindoApplication::DrawPromptBar(const Brain &brain) const {
  const int sw = GetScreenWidth();
  const int sh = GetScreenHeight();
  const int h = AppConfig::kPromptBarHeight;
  const int m = AppConfig::kPromptBarMargin;
  const int y = sh - h - m;
  const Rectangle bar = {static_cast<float>(m), static_cast<float>(y),
                         static_cast<float>(sw - 2 * m), static_cast<float>(h)};

  const Color fill = prompt_focus_ ? Fade(DARKBLUE, 0.85f) : Fade(BLACK, 0.6f);
  DrawRectangleRec(bar, fill);
  DrawRectangleLinesEx(bar, 2, prompt_focus_ ? SKYBLUE : Fade(LIGHTGRAY, 0.5f));

  const int fs = AppConfig::kPromptFontSize;
  const int tx = m + 8;
  const int ty = y + (h - fs) / 2;
  const char *prefix = prompt_focus_ ? "> " : "[TAB] ";
  DrawText(prefix, tx, ty, fs, LIGHTGRAY);

  const int prefix_w = MeasureText(prefix, fs);
  const std::string &line = prompt_input_;
  const char *display =
      line.empty() ? (prompt_focus_ ? "_" : "pergunta...") : line.c_str();
  Color text_col = line.empty() ? Fade(WHITE, 0.45f) : RAYWHITE;
  DrawText(display, tx + prefix_w, ty, fs, text_col);

  if (!brain.LlamaReady()) {
    DrawText(" (LLM offline)", tx + prefix_w + MeasureText(display, fs), ty,
             fs / 2, ORANGE);
  }
}

void AzulindoApplication::DrawStatusOverlay(const Brain &brain) const {
  DrawFPS(10, 50);
  if (!brain.LlamaReady()) {
    DrawText(AppConfig::kHintLlmOffline, 10, 32, 18, ORANGE);
  }
}

int AzulindoApplication::Run() {
  InitWindow(initial_width_, initial_height_, AppConfig::kWindowTitle);
  SetTargetFPS(AppConfig::kTargetFps);

  AzulindoScreen screen(initial_width_, initial_height_);
  Brain brain;

  while (!WindowShouldClose()) {
    const float dt = GetFrameTime();

    UpdatePromptInput(brain, screen);
    DrainTokensThrottled(brain, screen, dt);
    ApplyEmotionHotkeys(screen);

    const bool ai_speaking =
        brain.IsStreamingTokens() || screen.IsAiTextPipelineBusy();
    screen.SetPauseWaveWhileAiOutput(ai_speaking);

    screen.Update(dt);

    BeginDrawing();
    screen.Draw();
    DrawPromptBar(brain);
    DrawStatusOverlay(brain);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
