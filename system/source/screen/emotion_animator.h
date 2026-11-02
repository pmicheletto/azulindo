#ifndef AZULINDO_SCREEN_EMOTION_ANIMATOR_H_
#define AZULINDO_SCREEN_EMOTION_ANIMATOR_H_

#include "configs/wave_config.h"
#include "core/state_emotion/emotion.h"

class EmotionAnimator {
 public:
  EmotionAnimator();

  void SetEmotion(EmotionState emotion);
  void Update(float delta_time);

  const WaveConfig &wave_config() const { return wave_config_; }

 private:
  EmotionState current_emotion_ = EmotionState::kIdle;
  EmotionState target_emotion_ = EmotionState::kIdle;
  WaveConfig wave_config_{};
};

#endif  // AZULINDO_SCREEN_EMOTION_ANIMATOR_H_
