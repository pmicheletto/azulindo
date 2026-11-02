#include "screen/azulindo_screen.h"

#include "configs/layout_config.h"
#include "screen/hologram.h"

AzulindoScreen::AzulindoScreen(int screen_width, int screen_height)
    : screen_width_(screen_width),
      screen_height_(screen_height),
      use_full_screen_(screen_width <= 0 || screen_height <= 0),
      text_pipeline_(dialogue_),
      hologram_(std::make_unique<Hologram>(
          LayoutConfig::HologramConfig::kModelFilename)) {
  RefreshScreenSize();
  dialogue_.UpdateLayout(screen_width_, screen_height_);
}

AzulindoScreen::~AzulindoScreen() = default;

void AzulindoScreen::RefreshScreenSize() {
  if (use_full_screen_) {
    screen_width_ = GetScreenWidth();
    screen_height_ = GetScreenHeight();
  } else if (screen_width_ <= 0 || screen_height_ <= 0) {
    screen_width_ = GetScreenWidth();
    screen_height_ = GetScreenHeight();
  }
}

void AzulindoScreen::Update(float delta_time) {
  RefreshScreenSize();
  dialogue_.UpdateLayout(screen_width_, screen_height_);
  wave_.Update(delta_time);
  emotion_.Update(delta_time);
  hologram_->Update(delta_time);
}

void AzulindoScreen::Draw(SystemStatus system_status,
                          const std::string &llm_model) {
  ClearBackground(LayoutConfig::ColorConfig::background);
  DrawBackgroundLines();
  wave_.Draw(screen_width_, screen_height_, emotion_.wave_config());
  hud_.Draw(screen_width_, screen_height_, system_status, llm_model);
  dialogue_.Draw(text_pipeline_.text());
  hologram_->Draw(emotion_.wave_config().wave_color);
}

void AzulindoScreen::DrawBackgroundLines() const {
  for (int y = 0; y < screen_height_;
       y += LayoutConfig::BackgroundConfig::line_spacing) {
    DrawRectangle(0, y, screen_width_,
                  LayoutConfig::BackgroundConfig::line_thickness,
                  LayoutConfig::ColorConfig::background_line);
  }
}

void AzulindoScreen::SetEmotion(EmotionState emotion) {
  emotion_.SetEmotion(emotion);
}

void AzulindoScreen::AppendAiText(const std::string &text) {
  text_pipeline_.Append(text);
}

void AzulindoScreen::ClearAiText() { text_pipeline_.Clear(); }
