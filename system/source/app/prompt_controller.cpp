#include "app/prompt_controller.h"

#include "app/prompt_builder.h"
#include "app/utf8_input.h"
#include "configs/app_config.h"
#include "configs/prompt_config.h"
#include "core/brain/brain.h"
#include "core/state_emotion/emotion.h"
#include "raylib.h"
#include "screen/azulindo_screen.h"

bool PromptController::Update(Brain &brain, AzulindoScreen &screen,
                              PromptConfig::Language language) {
  language_ = language;

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
    if (!brain.IsReady()) {
      return false;
    }
    const std::string full =
        PromptBuilder::BuildChatml(input_, history_, language_);
    if (brain.RequestInference(full)) {
      pending_user_ = input_;
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

void PromptController::OnAssistantReply(std::string reply) {
  if (pending_user_.empty() || reply.empty()) {
    pending_user_.clear();
    return;
  }
  history_.push_back(
      PromptBuilder::Turn{std::move(pending_user_), std::move(reply)});
  pending_user_.clear();
  while (history_.size() > PromptConfig::kMaxHistoryTurns) {
    history_.erase(history_.begin());
  }
}

void PromptController::OnLanguageChanged() {
  history_.clear();
  pending_user_.clear();
}

void PromptController::Draw(const Brain &brain) const {
  const int sw = GetScreenWidth();
  const int sh = GetScreenHeight();
  const int h = AppConfig::kPromptBarHeight;
  const int m = AppConfig::kPromptBarMargin;
  const int y = sh - h - m;
  const Rectangle bar = {static_cast<float>(m), static_cast<float>(y),
                         static_cast<float>(sw - 2 * m), static_cast<float>(h)};

  const Color fill =
      focused_ ? Fade(DARKBLUE, AppConfig::kPromptFocusedAlpha)
               : Fade(BLACK, AppConfig::kPromptUnfocusedAlpha);
  DrawRectangleRec(bar, fill);
  DrawRectangleLinesEx(
      bar, AppConfig::kPromptBarBorderThickness,
      focused_ ? SKYBLUE : Fade(LIGHTGRAY, AppConfig::kPromptBorderUnfocusedAlpha));

  const int fs = AppConfig::kPromptFontSize;
  const int tx = m + AppConfig::kPromptBarPaddingX;
  const int ty = y + (h - fs) / 2;
  const char *prefix =
      focused_ ? AppConfig::kPromptPrefixFocused : AppConfig::kPromptPrefixIdle;
  DrawText(prefix, tx, ty, fs, LIGHTGRAY);

  const int prefix_w = MeasureText(prefix, fs);
  const char *placeholder =
      language_ == PromptConfig::Language::kPtBr ? "pergunte algo..."
                                                 : AppConfig::kPromptPlaceholder;
  const char *display =
      input_.empty()
          ? (focused_ ? AppConfig::kPromptCaret : placeholder)
          : input_.c_str();
  const Color text_col =
      input_.empty() ? Fade(WHITE, AppConfig::kPromptPlaceholderAlpha)
                     : RAYWHITE;
  DrawText(display, tx + prefix_w, ty, fs, text_col);

  if (!brain.IsReady()) {
    if (brain.IsLoading()) {
      DrawText(" (carregando...)", tx + prefix_w + MeasureText(display, fs), ty,
               fs / 2, SKYBLUE);
    } else {
      DrawText(AppConfig::kPromptOfflineSuffix,
               tx + prefix_w + MeasureText(display, fs), ty, fs / 2, ORANGE);
    }
  }
}
