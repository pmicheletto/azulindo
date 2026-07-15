#include "screen/emotion_animator.h"

#include <cmath>

#include "core/state_emotion/emotion_manager.h"
#include "core/state_emotion/emotion_profile.h"
#include "raymath.h"

EmotionAnimator::EmotionAnimator() {
  wave_config_ = GetEmotionProfile(current_emotion_).wave;
}

void EmotionAnimator::SetEmotion(EmotionState emotion) {
  EmotionManager::Instance().SetState(emotion);
}

void EmotionAnimator::Update(float delta_time) {
  target_emotion_ = EmotionManager::Instance().GetCurrentState();
  const EmotionProfile &target = GetEmotionProfile(target_emotion_);
  const float factor = delta_time * target.transition_speed;

  auto update = [factor](float &current, float target_val) {
    current = Lerp(current, target_val, factor);
  };

  update(wave_config_.wave_density, target.wave.wave_density);
  update(wave_config_.amplitude_min, target.wave.amplitude_min);
  update(wave_config_.amplitude_max, target.wave.amplitude_max);
  update(wave_config_.harmonic_strength, target.wave.harmonic_strength);
  update(wave_config_.harmonic_frequency, target.wave.harmonic_frequency);
  update(wave_config_.animation_speed, target.wave.animation_speed);
  update(wave_config_.harmonic_speed, target.wave.harmonic_speed);

  wave_config_.wave_color =
      ColorLerp(wave_config_.wave_color, target.wave.wave_color, factor);
  wave_config_.glow_color =
      ColorLerp(wave_config_.glow_color, target.wave.glow_color, factor);

  if (std::abs(wave_config_.wave_density - target.wave.wave_density) < 0.001f) {
    current_emotion_ = target_emotion_;
  }
}
