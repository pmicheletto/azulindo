#ifndef AZULINDO_SCREEN_H_
#define AZULINDO_SCREEN_H_

#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>

#include "core/emotion_profile.h"
#include "hologram.h"
#include "configs/layout_config.h"
#include "raylib.h"
#include "raymath.h"

class AzulindoScreen {
 public:
  AzulindoScreen(int screen_width, int screen_height);

  void Update(float delta_time);
  void Draw();
  void SetEmotion(EmotionState emotion);

 private:
  void UpdateLayout();
  void UpdateEmotion(float delta_time);

  void DrawBackgroundLines() const;
  void DrawDialogueBox() const;
  void DrawHud() const;
  void DrawWave() const;
  void DrawTextWrapped(Font font, const char* text, Rectangle rec,
                       float fontSize, float spacing, Color color) const;

  int screen_width_;
  int screen_height_;
  bool use_full_screen_;
  float timer_ = 0.0f;

  char ai_text_[1024] =
      "AZULINDO: Sistema inicializado. Aguardando comandos...";

  Rectangle dialogue_bounds_;
  EmotionState current_emotion_ = EmotionState::kIdle;
  EmotionState target_emotion_ = EmotionState::kIdle;

  WaveConfig wave_config_;

  std::unique_ptr<Hologram> car_hologram_;
};

#endif  // AZULINDO_SCREEN_H_
