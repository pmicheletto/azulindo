#include "screen/azulindo_screen.h"

#include <cmath>

#include "raylib.h"
#include "raymath.h"

AzulindoScreen::AzulindoScreen(int screen_width, int screen_height)
    : screen_width_(screen_width),
      screen_height_(screen_height) {
  wave_config_ = GetEmotionProfile(current_emotion_).wave;
}

void AzulindoScreen::SetEmotion(EmotionState emotion) {
  target_emotion_ = emotion;
}

void AzulindoScreen::Update(float delta_time) {
  timer_ += delta_time;
  UpdateEmotion(delta_time);
}

void AzulindoScreen::Draw() {
  ClearBackground({2, 8, 20, 255});

  DrawBackgroundLines();
  DrawWave();
  DrawHud();
}

void AzulindoScreen::UpdateEmotion(float dt) {
  const EmotionProfile& target =
      GetEmotionProfile(target_emotion_);

  const float speed = target.transition_speed;

  wave_config_.frequency = Lerp(
      wave_config_.frequency,
      target.wave.frequency,
      dt * speed);

  wave_config_.speed = Lerp(
      wave_config_.speed,
      target.wave.speed,
      dt * speed);

  wave_config_.base_intensity = Lerp(
      wave_config_.base_intensity,
      target.wave.base_intensity,
      dt * speed);

  wave_config_.max_intensity = Lerp(
      wave_config_.max_intensity,
      target.wave.max_intensity,
      dt * speed);

  wave_config_.core_color = ColorLerp(
      wave_config_.core_color,
      target.wave.core_color,
      dt * speed);

  wave_config_.glow_color = ColorLerp(
      wave_config_.glow_color,
      target.wave.glow_color,
      dt * speed);

  if (fabs(wave_config_.frequency - target.wave.frequency) < 0.0001f &&
      fabs(wave_config_.speed - target.wave.speed) < 0.0001f) {
    current_emotion_ = target_emotion_;
  }
}

void AzulindoScreen::DrawBackgroundLines() const {
  for (int y = 0; y < screen_height_; y += 4) {
    DrawRectangle(0, y, screen_width_, 1, {50, 100, 255, 12});
  }
}

void AzulindoScreen::DrawHud() const {
  DrawText("SYSTEM: AZULINDO ACTIVE", 20, 20, 15, SKYBLUE);
  DrawText("AI ENGINE: ONLINE", 20, 40, 12, BLUE);

  DrawRectangle(screen_width_ - 200, 20, 180, 80, {30, 60, 150, 40});
  DrawText("AZULINDO LOGS:", screen_width_ - 190, 30, 10, SKYBLUE);

  DrawText("- Emotional Core: OK",
           screen_width_ - 190, 50, 10, LIGHTGRAY);
  DrawText("- Wave Engine: STABLE",
           screen_width_ - 190, 65, 10, LIGHTGRAY);
}

void AzulindoScreen::DrawWave() const {
  const int center_y = screen_height_ / 2;

  const float pulse =
      sinf(timer_ * wave_config_.speed) * 0.1f + 0.9f;

  const float intensity =
      wave_config_.base_intensity +
      (wave_config_.max_intensity - wave_config_.base_intensity) * pulse;

  for (int x = 0; x < screen_width_; x += 2) {
    const float phase =
        x * wave_config_.frequency + timer_ * wave_config_.speed;

    float y1 = cosf(phase) * intensity;
    float y2 = cosf(phase + PI / 2) * intensity;

    const float harmonic =
        cosf(x * 0.05f - timer_ * 2.0f) * (intensity * 0.02f);

    y1 += harmonic;
    y2 -= harmonic;

    const int pos_y1 = center_y + static_cast<int>(y1);
    const int pos_y2 = center_y + static_cast<int>(y2);

    DrawLine(x, pos_y2, x, pos_y1,
             Fade(wave_config_.core_color, 0.15f));
  }
}
