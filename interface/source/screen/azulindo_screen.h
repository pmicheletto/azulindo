#include "core/emotion.h"
#include "core/emotion_profile.h"

class AzulindoScreen {
 public:
  AzulindoScreen(int screen_width, int screen_height);

  void Update(float delta_time);
  void Draw();
  void SetEmotion(EmotionState emotion);

 private:
  void DrawBackgroundLines() const;
  void DrawHud() const;
  void DrawWave() const;
  void UpdateEmotion(float delta_time);

  int screen_width_;
  int screen_height_;

  float timer_ = 0.0f;
  float intensity_;

  EmotionState current_emotion_ = EmotionState::kIdle;
  EmotionState target_emotion_ = EmotionState::kIdle;

  WaveConfig wave_config_;
};

