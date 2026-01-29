#include "core/emotion.h"
#include "raylib.h"
#include "screen/azulindo_screen.h"

int main() {
  int current_width = 800;
  int current_height = 480;

  InitWindow(current_width, current_height, "Azulindo");
  SetTargetFPS(60);

  AzulindoScreen screen(current_width, current_height);

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
