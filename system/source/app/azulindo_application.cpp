#include "app/azulindo_application.h"

#include "app/emotion_hotkeys.h"
#include "configs/app_config.h"
#include "core/brain/brain.h"
#include "raylib.h"
#include "screen/azulindo_screen.h"

AzulindoApplication::AzulindoApplication(int initial_width, int initial_height)
    : initial_width_(initial_width), initial_height_(initial_height) {}

int AzulindoApplication::Run() {
  InitWindow(initial_width_, initial_height_, AppConfig::kWindowTitle);
  SetTargetFPS(AppConfig::kTargetFps);

  AzulindoScreen screen(initial_width_, initial_height_);
  Brain brain;

  while (!WindowShouldClose()) {
    const float dt = GetFrameTime();

    if (prompt_.Update(brain, screen)) {
      tokens_.ResetCooldown();
    }
    tokens_.Update(brain, screen, dt);
    EmotionHotkeys::Apply(screen);

    screen.Update(dt);

    BeginDrawing();
    screen.Draw();
    prompt_.Draw(brain);
    status_.Draw(brain);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
