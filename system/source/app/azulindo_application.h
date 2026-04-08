#ifndef AZULINDO_APP_AZULINDO_APPLICATION_H_
#define AZULINDO_APP_AZULINDO_APPLICATION_H_

#include <string>

#include "configs/app_config.h"

class Brain;
class AzulindoScreen;

class AzulindoApplication {
 public:
  explicit AzulindoApplication(
      int initial_width = AppConfig::kDefaultWindowWidth,
      int initial_height = AppConfig::kDefaultWindowHeight);

  int Run();

 private:
  void DrainTokensThrottled(Brain &brain, AzulindoScreen &screen, float dt);
  void UpdatePromptInput(Brain &brain, AzulindoScreen &screen);
  void DrawPromptBar(const Brain &brain) const;
  void DrawStatusOverlay(const Brain &brain) const;

  int initial_width_;
  int initial_height_;

  std::string prompt_input_;
  float token_display_cooldown_ = 0.0f;
  bool prompt_focus_ = false;
};

#endif
