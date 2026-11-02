#include "app/language_selector.h"

#include "configs/layout_config.h"

LanguageSelector::HitZones LanguageSelector::ComputeHitZones(
    int screen_width) const {
  using Cfg = LayoutConfig::LanguageSelectorConfig;
  const int fs = Cfg::font_size;
  const int br_w = MeasureText(Cfg::kLabelBr, fs);
  const int sep_w = MeasureText(Cfg::kLabelSeparator, fs);
  const int en_w = MeasureText(Cfg::kLabelEn, fs);
  const float total_w = static_cast<float>(br_w) + Cfg::separator_gap +
                        static_cast<float>(sep_w) + Cfg::separator_gap +
                        static_cast<float>(en_w);
  const float start_x = (static_cast<float>(screen_width) - total_w) * 0.5f;
  const float y = Cfg::top_margin;
  const float h = static_cast<float>(fs);

  const float br_x = start_x;
  const float sep_x = br_x + static_cast<float>(br_w) + Cfg::separator_gap;
  const float en_x = sep_x + static_cast<float>(sep_w) + Cfg::separator_gap;

  return {
      Rectangle{br_x - Cfg::hit_padding_x, y - Cfg::hit_padding_y,
                static_cast<float>(br_w) + 2.0f * Cfg::hit_padding_x,
                h + 2.0f * Cfg::hit_padding_y},
      Rectangle{en_x - Cfg::hit_padding_x, y - Cfg::hit_padding_y,
                static_cast<float>(en_w) + 2.0f * Cfg::hit_padding_x,
                h + 2.0f * Cfg::hit_padding_y},
  };
}

bool LanguageSelector::Update() {
  if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    return false;
  }

  const HitZones zones = ComputeHitZones(GetScreenWidth());
  const Vector2 mouse = GetMousePosition();

  if (CheckCollisionPointRec(mouse, zones.br) &&
      language_ != PromptConfig::Language::kPtBr) {
    language_ = PromptConfig::Language::kPtBr;
    return true;
  }
  if (CheckCollisionPointRec(mouse, zones.en) &&
      language_ != PromptConfig::Language::kEn) {
    language_ = PromptConfig::Language::kEn;
    return true;
  }
  return false;
}

void LanguageSelector::Draw() const {
  using Cfg = LayoutConfig::LanguageSelectorConfig;
  const int sw = GetScreenWidth();
  const int fs = Cfg::font_size;
  const int br_w = MeasureText(Cfg::kLabelBr, fs);
  const int sep_w = MeasureText(Cfg::kLabelSeparator, fs);
  const float total_w = static_cast<float>(br_w) + Cfg::separator_gap +
                        static_cast<float>(sep_w) + Cfg::separator_gap +
                        static_cast<float>(MeasureText(Cfg::kLabelEn, fs));
  const int start_x =
      static_cast<int>((static_cast<float>(sw) - total_w) * 0.5f);
  const int y = static_cast<int>(Cfg::top_margin);

  const bool br_active = language_ == PromptConfig::Language::kPtBr;
  const Color br_color = br_active ? SKYBLUE : Fade(LIGHTGRAY, 0.45f);
  const Color en_color = br_active ? Fade(LIGHTGRAY, 0.45f) : SKYBLUE;
  const Color sep_color = Fade(LIGHTGRAY, 0.55f);

  int x = start_x;
  DrawText(Cfg::kLabelBr, x, y, fs, br_color);
  x += br_w + static_cast<int>(Cfg::separator_gap);
  DrawText(Cfg::kLabelSeparator, x, y, fs, sep_color);
  x += sep_w + static_cast<int>(Cfg::separator_gap);
  DrawText(Cfg::kLabelEn, x, y, fs, en_color);
}
