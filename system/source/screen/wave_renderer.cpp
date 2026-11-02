#include "screen/wave_renderer.h"

#include <algorithm>
#include <cmath>

#include "configs/layout_config.h"
#include "raylib.h"

void WaveRenderer::Update(float delta_time) { timer_ += delta_time; }

void WaveRenderer::Draw(int screen_width, int screen_height,
                        const WaveConfig &config) const {
  const float vertical_offset =
      std::clamp(static_cast<float>(screen_height) *
                     LayoutConfig::WaveLayoutConfig::vertical_offset_percent,
                 LayoutConfig::WaveLayoutConfig::vertical_offset_min,
                 LayoutConfig::WaveLayoutConfig::vertical_offset_max);
  const int wave_position_y =
      (screen_height / 2) - static_cast<int>(vertical_offset);
  const int wave_position_x = (screen_width / 2);

  const float pulse_min = LayoutConfig::WaveLayoutConfig::pulse_min;
  const float pulse_max = LayoutConfig::WaveLayoutConfig::pulse_max;
  const float pulse =
      sinf(timer_ * config.animation_speed) * pulse_min + pulse_max;

  const float amplitude =
      config.amplitude_min +
      (config.amplitude_max - config.amplitude_min) * pulse;

  const int x_step = LayoutConfig::WaveLayoutConfig::x_step;
  for (int x = 0; x < screen_width; x += x_step) {
    float h1 =
        sinf(x * config.wave_density + timer_ * config.animation_speed) *
        amplitude;
    float h2 = sinf(x * config.wave_density + timer_ * config.animation_speed +
                    LayoutConfig::WaveLayoutConfig::phase_offset) *
               amplitude;

    float harmonic =
        sinf(x * config.harmonic_frequency - timer_ * config.harmonic_speed) *
        (amplitude * config.harmonic_strength);
    h1 += harmonic;
    h2 -= harmonic;

    const int pos_y1 = wave_position_y + static_cast<int>(h1);
    const int pos_y2 = wave_position_y + static_cast<int>(h2);
    const int pos_x1 = wave_position_x + static_cast<int>(h2);

    DrawLine(pos_x1, pos_y1, x, pos_y2,
             Fade(config.wave_color, LayoutConfig::WaveLayoutConfig::fade_alpha));
  }
}
