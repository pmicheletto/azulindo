#include "app/token_stream_controller.h"

#include <string>

#include "configs/app_config.h"
#include "core/brain/brain.h"
#include "core/state_emotion/emotion.h"
#include "screen/azulindo_screen.h"

void TokenStreamController::ResetCooldown() {
  display_cooldown_ = 0.0f;
  speech_idle_applied_ = false;
}

void TokenStreamController::Update(Brain &brain, AzulindoScreen &screen,
                                   float delta_time) {
  display_cooldown_ -= delta_time;
  if (display_cooldown_ > 0.0f) {
    return;
  }
  std::string token = brain.PopToken();
  if (token.empty()) {
    display_cooldown_ = 0.0f;
    if (!brain.IsStreamingTokens()) {
      speech_idle_applied_ = false;
    }
    return;
  }
  screen.AppendAiText(token);
  if (!speech_idle_applied_) {
    screen.SetEmotion(EmotionState::kIdle);
    speech_idle_applied_ = true;
  }
  display_cooldown_ = AppConfig::kUiTokenIntervalSeconds;
}
