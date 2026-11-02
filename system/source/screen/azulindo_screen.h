#ifndef AZULINDO_SCREEN_AZULINDO_SCREEN_H_
#define AZULINDO_SCREEN_AZULINDO_SCREEN_H_

#include <memory>
#include <string>

#include "core/state_emotion/emotion.h"
#include "screen/ai_text_pipeline.h"
#include "screen/dialogue_panel.h"
#include "screen/emotion_animator.h"
#include "screen/hud_panel.h"
#include "screen/wave_renderer.h"

class Hologram;

class AzulindoScreen {
 public:
  AzulindoScreen(int screen_width, int screen_height);
  ~AzulindoScreen();

  void Update(float delta_time);
  void Draw(SystemStatus system_status, const std::string &llm_model);

  void SetEmotion(EmotionState emotion);
  void AppendAiText(const std::string &text);
  void ClearAiText();

 private:
  void RefreshScreenSize();
  void DrawBackgroundLines() const;

  int screen_width_;
  int screen_height_;
  bool use_full_screen_;

  EmotionAnimator emotion_;
  WaveRenderer wave_;
  HudPanel hud_;
  DialoguePanel dialogue_;
  AiTextPipeline text_pipeline_;
  std::unique_ptr<Hologram> hologram_;
};

#endif  // AZULINDO_SCREEN_AZULINDO_SCREEN_H_
