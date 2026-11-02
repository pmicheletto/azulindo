#include "app/azulindo_application.h"

#include <string>

#include "app/emotion_hotkeys.h"
#include "app/system_status_mapper.h"
#include "configs/app_config.h"
#include "configs/prompt_config.h"
#include "core/brain/brain.h"
#include "raylib.h"
#include "screen/azulindo_screen.h"

namespace {

void DrawBootFrame(const char *message) {
  BeginDrawing();
  ClearBackground(BLACK);
  const int font_size = 22;
  const int text_w = MeasureText(message, font_size);
  DrawText(message, (GetScreenWidth() - text_w) / 2,
           GetScreenHeight() / 2 - font_size / 2, font_size, SKYBLUE);
  EndDrawing();
}

}  // namespace

AzulindoApplication::AzulindoApplication(int initial_width, int initial_height)
    : initial_width_(initial_width), initial_height_(initial_height) {}

int AzulindoApplication::Run(int argc, char **argv) {
  InitWindow(initial_width_, initial_height_, AppConfig::kWindowTitle);
  SetTargetFPS(AppConfig::kTargetFps);

  DrawBootFrame(AppConfig::kHintBootScene);

  AzulindoScreen screen(initial_width_, initial_height_);

  std::string model_path;
  if (argc > 1 && argv != nullptr && argv[1] != nullptr) {
    model_path = argv[1];
  }
  Brain brain(std::move(model_path));

  while (!WindowShouldClose()) {
    const float dt = GetFrameTime();

    if (language_.Update()) {
      brain.SetPersonality(
          PromptConfig::PersonalityFor(language_.language()));
      prompt_.OnLanguageChanged();
      screen.ClearAiText();
    }

    if (prompt_.Update(brain, screen, language_.language())) {
      tokens_.ResetCooldown();
    }
    tokens_.Update(brain, screen, dt);
    if (auto reply = tokens_.TakeCompletedReply()) {
      prompt_.OnAssistantReply(std::move(*reply));
    }
    EmotionHotkeys::Apply(screen);

    screen.Update(dt);

    BeginDrawing();
    screen.Draw(SystemStatusMapper::FromBrain(brain),
                brain.ModelDisplayName());
    language_.Draw();
    prompt_.Draw(brain);
    status_.Draw();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
