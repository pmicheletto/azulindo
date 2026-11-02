#include "app/token_stream_controller.h"

#include <string>
#include <utility>

#include "app/utf8_input.h"
#include "configs/app_config.h"
#include "core/brain/brain.h"
#include "core/state_emotion/emotion.h"
#include "screen/azulindo_screen.h"

void TokenStreamController::ResetCooldown() {
  display_cooldown_ = 0.0f;
  speech_idle_applied_ = false;
  accumulating_.clear();
  think_filter_.Reset();
  was_streaming_ = true;
}

void TokenStreamController::Update(Brain &brain, AzulindoScreen &screen,
                                   float delta_time) {
  display_cooldown_ -= delta_time;
  if (display_cooldown_ > 0.0f) {
    return;
  }

  std::string token = brain.PopToken();
  const bool streaming = brain.IsStreamingTokens() || !token.empty();

  if (!token.empty()) {
    const std::string visible =
        Utf8Input::SanitizeForHudFont(think_filter_.Feed(token));
    if (!visible.empty()) {
      accumulating_ += visible;
      screen.AppendAiText(visible);
      if (!speech_idle_applied_) {
        screen.SetEmotion(EmotionState::kIdle);
        speech_idle_applied_ = true;
      }
    }
    display_cooldown_ = AppConfig::kUiTokenIntervalSeconds;
  } else {
    display_cooldown_ = 0.0f;
    if (!streaming) {
      speech_idle_applied_ = false;
    }
  }

  if (was_streaming_ && !streaming) {
    const std::string tail =
        Utf8Input::SanitizeForHudFont(think_filter_.Flush());
    if (!tail.empty()) {
      accumulating_ += tail;
      screen.AppendAiText(tail);
    }
    if (!accumulating_.empty()) {
      completed_reply_ = std::move(accumulating_);
      accumulating_.clear();
    }
    think_filter_.Reset();
  }
  was_streaming_ = streaming;
}

std::optional<std::string> TokenStreamController::TakeCompletedReply() {
  if (!completed_reply_.has_value()) {
    return std::nullopt;
  }
  std::optional<std::string> out = std::move(completed_reply_);
  completed_reply_.reset();
  return out;
}
