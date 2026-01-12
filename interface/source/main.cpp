#include "core/emotion.h"
#include "raylib.h"
#include "screen/azulindo_screen.h"

int main() {
  InitWindow(480, 480, "Azulindo");
  SetTargetFPS(60);

  AzulindoScreen screen(480, 480);

  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_ONE)) {
      screen.SetEmotion(EmotionState::kIdle);

    } else if (IsKeyDown(KEY_TWO)) {
      screen.SetEmotion(EmotionState::kAngry);

    } else if (IsKeyDown(KEY_THREE)) {
      screen.SetEmotion(EmotionState::kThinking);
    }

    screen.Update(GetFrameTime());

    BeginDrawing();
    screen.Draw();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
