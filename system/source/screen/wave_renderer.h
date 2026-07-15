#ifndef AZULINDO_SCREEN_WAVE_RENDERER_H_
#define AZULINDO_SCREEN_WAVE_RENDERER_H_

#include "configs/wave_config.h"

class WaveRenderer {
 public:
  void ResetTimer();
  void SetPaused(bool paused);
  void Update(float delta_time);
  void Draw(int screen_width, int screen_height,
            const WaveConfig &config) const;

 private:
  float timer_ = 0.0f;
  bool paused_ = false;
};

#endif  // AZULINDO_SCREEN_WAVE_RENDERER_H_
