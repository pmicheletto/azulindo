#ifndef AZULINDO_APP_AZULINDO_APPLICATION_H_
#define AZULINDO_APP_AZULINDO_APPLICATION_H_

#include "app/language_selector.h"
#include "app/prompt_controller.h"
#include "app/status_overlay.h"
#include "app/token_stream_controller.h"
#include "configs/app_config.h"

class AzulindoApplication {
 public:
  explicit AzulindoApplication(
      int initial_width = AppConfig::kDefaultWindowWidth,
      int initial_height = AppConfig::kDefaultWindowHeight);

  int Run(int argc = 0, char **argv = nullptr);

 private:
  int initial_width_;
  int initial_height_;

  LanguageSelector language_;
  PromptController prompt_;
  TokenStreamController tokens_;
  StatusOverlay status_;
};

#endif  // AZULINDO_APP_AZULINDO_APPLICATION_H_
