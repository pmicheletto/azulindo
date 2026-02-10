#include "emotion_profile.h"

#include "raylib.h"

namespace {

const EmotionProfile kIdleProfile = {
    .wave =
        {
            .wave_density = 0.02f,        // Gentle, wide waves
            .amplitude_min = 10.0f,       // Subtle baseline
            .amplitude_max = 30.0f,       // Gentle pulsing
            .harmonic_strength = 0.2f,    // Moderate harmonic effect
            .harmonic_frequency = 0.05f,  // Default harmonic frequency
            .animation_speed = 1.0f,      // Calm, slow animation
            .harmonic_speed = 2.0f,       // Harmonic moves twice as fast
            .wave_color = SKYBLUE,        // Calm blue
            .glow_color = {80, 160, 255, 200},
        },
    .transition_speed = 1.0f,
};

const EmotionProfile kThinkingProfile = {
    .wave =
        {
            .wave_density = 0.05f,        // More frequent waves (thinking)
            .amplitude_min = 20.0f,       // Moderate baseline
            .amplitude_max = 40.0f,       // More dynamic pulsing
            .harmonic_strength = 0.2f,    // Default harmonic
            .harmonic_frequency = 0.05f,  // Default harmonic frequency
            .animation_speed = 3.0f,      // Faster, more active
            .harmonic_speed = 2.0f,       // Default harmonic speed
            .wave_color = GREEN,           // Default color
            .glow_color = {80, 160, 255, 200},
        },
    .transition_speed = 2.0f,
};

const EmotionProfile kAngryProfile = {
    .wave =
        {
            .wave_density = 0.08f,        // Tight, frequent waves (intense)
            .amplitude_min = 40.0f,       // High baseline
            .amplitude_max = 80.0f,       // Dramatic pulsing
            .harmonic_strength = 0.2f,    // Default harmonic
            .harmonic_frequency = 0.05f,  // Default harmonic frequency
            .animation_speed = 7.0f,      // Very fast, energetic
            .harmonic_speed = 2.0f,       // Default harmonic speed
            .wave_color = RED,            // Angry red color
            .glow_color = {255, 80, 80, 200},
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
