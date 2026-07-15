#include "app/prompt_controller.h"

#include "app/prompt_builder.h"
#include "app/utf8_input.h"
#include "configs/app_config.h"
#include "core/brain/brain.h"
#include "core/state_emotion/emotion.h"
#include "raylib.h"
#include "screen/azulindo_screen.h"

bool PromptController::Update(Brain &brain, AzulindoScreen &screen) {
  if (IsKeyPressed(KEY_TAB)) {
    focused_ = !focused_;
    return false;
  }

  if (!focused_) {
    return false;
  }

  if (IsKeyPressed(KEY_ESCAPE)) {
    focused_ = false;
    input_.clear();
    return false;
  }

  const bool enter = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
  if (enter) {
    if (input_.empty()) {
      focused_ = false;
      return false;
    }
    if (!brain.LlamaReady()) {
      return false;
    }
    const std::string full = PromptBuilder::BuildChatml(input_);
    if (brain.RequestInference(full)) {
      screen.ClearAiText();
      screen.SetEmotion(EmotionState::kThinking);
      input_.clear();
      focused_ = false;
      return true;
    }
    return false;
  }

  if (IsKeyPressed(KEY_BACKSPACE)) {
    Utf8Input::PopLastChar(&input_);
  }

  int key = 0;
  while ((key = GetCharPressed()) > 0) {
    if (key == '\n' || key == '\r') {
      continue;
    }
    if (static_cast<size_t>(input_.size()) >= AppConfig::kMaxPromptInputChars) {
      break;
    }
    Utf8Input::AppendCodepoint(&input_, key);
  }
  return false;
}

void PromptController::Draw(const Brain &brain) const {
  const int sw = GetScreenWidth();
  const int sh = GetScreenHeight();
  const int h = AppConfig::kPromptBarHeight;
  const int m = AppConfig::kPromptBarMargin;
  const int y = sh - h - m;
  const Rectangle bar = {static_cast<float>(m), static_cast<float>(y),
                         static_cast<float>(sw - 2 * m), static_cast<float>(h)};

  const Color fill = focused_ ? Fade(DARKBLUE, 0.85f) : Fade(BLACK, 0.6f);
  DrawRectangleRec(bar, fill);
  DrawRectangleLinesEx(bar, 2, focused_ ? SKYBLUE : Fade(LIGHTGRAY, 0.5f));

  const int fs = AppConfig::kPromptFontSize;
  const int tx = m + 8;
  const int ty = y + (h - fs) / 2;
  const char *prefix = focused_ ? "> " : "[TAB] ";
  DrawText(prefix, tx, ty, fs, LIGHTGRAY);

  const int prefix_w = MeasureText(prefix, fs);
  const char *display =
      input_.empty() ? (focused_ ? "_" : "pergunta...") : input_.c_str();
  Color text_col = input_.empty() ? Fade(WHITE, 0.45f) : RAYWHITE;
  DrawText(display, tx + prefix_w, ty, fs, text_col);

  if (!brain.LlamaReady()) {
    DrawText(" (LLM offline)", tx + prefix_w + MeasureText(display, fs), ty,
             fs / 2, ORANGE);
  }
}
