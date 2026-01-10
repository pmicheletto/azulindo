#include "emotion_profile.h"

#include "raylib.h"

namespace {

const EmotionProfile kIdleProfile = {
    .wave = {
        .frequency = 0.02f,
        .speed = 1.0f,
        .base_intensity = 10.0f,
        .max_intensity = 20.0f,
        .use_sin_wave = true,
        .harmonic_coefficient = 0.2f,
    },
    .transition_speed = 1.0f,
};

const EmotionProfile kThinkingProfile = {
    .wave = {
        .frequency = 0.05f,
        .speed = 3.0f,
        .base_intensity = 20.0f,
        .max_intensity = 40.0f,
    },
    .transition_speed = 2.0f,
};

const EmotionProfile kAngryProfile = {
    .wave = {
        .frequency = 0.08f,
        .speed = 6.0f,
        .core_color = RED,
        .base_intensity = 40.0f,
        .max_intensity = 80.0f,
    },
    .transition_speed = 4.0f,
};

}  // namespace

const EmotionProfile& GetEmotionProfile(EmotionState state) {
  switch (state) {
    case EmotionState::kIdle:
      return kIdleProfile;
    case EmotionState::kThinking:
      return kThinkingProfile;
    case EmotionState::kAngry:
      return kAngryProfile;
    default:
      return kIdleProfile;
  }
}


