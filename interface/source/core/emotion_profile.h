#ifndef AZULINDO_EMOTION_PROFILE_H_
#define AZULINDO_EMOTION_PROFILE_H_

#include "configs/wave_config.h"
#include "emotion.h"

struct EmotionProfile {
  WaveConfig wave;
  float transition_speed = 2.0f;
};

const EmotionProfile& GetEmotionProfile(EmotionState state);

#endif  // AZULINDO_EMOTION_PROFILE_H_
