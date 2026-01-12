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
  const EmotionProfile& target = GetEmotionProfile(target_emotion_);

  const float speed = target.transition_speed;

  wave_config_.wave_density =
      Lerp(wave_config_.wave_density, target.wave.wave_density, dt * speed);

  wave_config_.amplitude_min =
      Lerp(wave_config_.amplitude_min, target.wave.amplitude_min, dt * speed);

  wave_config_.amplitude_max =
      Lerp(wave_config_.amplitude_max, target.wave.amplitude_max, dt * speed);

  wave_config_.harmonic_strength =
      Lerp(wave_config_.harmonic_strength, target.wave.harmonic_strength,
           dt * speed);

  wave_config_.harmonic_frequency =
      Lerp(wave_config_.harmonic_frequency, target.wave.harmonic_frequency,
           dt * speed);

  wave_config_.animation_speed = Lerp(wave_config_.animation_speed,
                                      target.wave.animation_speed, dt * speed);

  wave_config_.harmonic_speed =
      Lerp(wave_config_.harmonic_speed, target.wave.harmonic_speed, dt * speed);

  wave_config_.wave_color =
      ColorLerp(wave_config_.wave_color, target.wave.wave_color, dt * speed);

  wave_config_.glow_color =
      ColorLerp(wave_config_.glow_color, target.wave.glow_color, dt * speed);

  if (fabs(wave_config_.wave_density - target.wave.wave_density) < 0.0001f &&
      fabs(wave_config_.animation_speed - target.wave.animation_speed) <
          0.0001f) {
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
    float y1 = sinf(x * wave_config_.frequency + timer_ * wave_config_.speed) * intensity;
    float y2 = sinf(x * wave_config_.frequency + timer_ * wave_config_.speed + PI / 2) * intensity;

    float harmonic = sinf(x * 0.05f - timer_ * 2.0f) * (intensity * 0.2f);
    y1 += harmonic;
    y2 -= harmonic;

    int pos_y1 = center_y + static_cast<int>(y1);
    int pos_y2 = center_y + static_cast<int>(y2);

    DrawLine(pos_y2, pos_y1, x, pos_y2, Fade(wave_config_.core_color, 0.1f));

    for (int layer = 1; layer <= 3; ++layer) {
      float radius = static_cast<float>(4 - layer);
      Color layer_color = Fade(wave_config_.glow_color, 0.1f / layer);
      
      // DrawCircle(x, pos_y1, radius, layer_color);
      // DrawCircle(x, pos_y2, radius, layer_color);
    }
  }
}
