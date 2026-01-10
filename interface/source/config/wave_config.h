#ifndef AZULINDO_WAVE_CONFIG_H_
#define AZULINDO_WAVE_CONFIG_H_

#include "raylib.h"

struct WaveConfig {
  float frequency = 0.04f;
  float speed = 5.0f;

  Color core_color = SKYBLUE;
  Color glow_color = {80, 160, 255, 200};

  float base_intensity = 25.0f;
  float max_intensity = 100.0f;
  
  bool use_sin_wave = false;  // false = cos, true = sin
  float harmonic_coefficient = 0.02f;
};

#endif  // AZULINDO_WAVE_CONFIG_H_
