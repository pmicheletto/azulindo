#ifndef AZULINDO_APP_LANGUAGE_SELECTOR_H_
#define AZULINDO_APP_LANGUAGE_SELECTOR_H_

#include "configs/prompt_config.h"
#include "raylib.h"

class LanguageSelector {
 public:
  PromptConfig::Language language() const { return language_; }

  // Returns true when the active language changed via click.
  bool Update();
  void Draw() const;

 private:
  struct HitZones {
    Rectangle br;
    Rectangle en;
  };

  HitZones ComputeHitZones(int screen_width) const;

  PromptConfig::Language language_ = PromptConfig::kDefaultLanguage;
};

#endif  // AZULINDO_APP_LANGUAGE_SELECTOR_H_
