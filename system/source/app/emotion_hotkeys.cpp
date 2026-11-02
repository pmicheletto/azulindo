#include "app/emotion_hotkeys.h"

#include "core/state_emotion/emotion.h"
#include "raylib.h"
#include "screen/azulindo_screen.h"

namespace EmotionHotkeys {

void Apply(AzulindoScreen &screen) {
  if (IsKeyPressed(KEY_ONE)) {
    screen.SetEmotion(EmotionState::kIdle);
  } else if (IsKeyPressed(KEY_TWO)) {
    screen.SetEmotion(EmotionState::kAngry);
  } else if (IsKeyPressed(KEY_THREE)) {
    screen.SetEmotion(EmotionState::kThinking);
  }
}

}  // namespace EmotionHotkeys
