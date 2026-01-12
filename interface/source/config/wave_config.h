#ifndef AZULINDO_WAVE_CONFIG_H_
#define AZULINDO_WAVE_CONFIG_H_

#include "raylib.h"

struct WaveConfig {
  float wave_density = 0.04f;
  float amplitude_min = 25.0f;
  float amplitude_max = 100.0f;
  float harmonic_strength = 0.2f;
  float harmonic_frequency = 0.05f;  // Controls the frequency of the harmonic
                                     // wave (how fast it oscillates)

  float animation_speed = 5.0f;
  float harmonic_speed = 2.0f;  // Controls how fast the harmonic wave animates

  Color wave_color = SKYBLUE;
  Color glow_color = {80, 160, 255, 200};
};

#endif  // AZULINDO_WAVE_CONFIG_H_
