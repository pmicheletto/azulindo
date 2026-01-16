#ifndef AZULINDO_SCREEN_H_
#define AZULINDO_SCREEN_H_

#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <memory>
#include <string>

#include "core/emotion_profile.h"
#include "hologram.h"
#include "raylib.h"
#include "raymath.h"

namespace LayoutConstants {
struct BackgroundConfig {
  static constexpr int line_spacing = 4;
  static constexpr int line_thickness = 1;
};

struct ColorConfig {
  static constexpr Color background = {2, 8, 20, 255};
  static constexpr Color background_line = {50, 100, 255, 12};
  static constexpr Color dialogue_box = {30, 60, 150, 40};
  static constexpr Color dialogue_border = {50, 100, 255, 100};
  static constexpr Color hud_panel = {30, 60, 150, 40};
};

struct DialogueConfig {
  // Responsive: use percentages of screen dimensions
  static constexpr float position_x_percent = 0.03f;         // 3% from left
  static constexpr float position_y_offset_percent = 0.15f;  // 15% from bottom
  static constexpr float width_percent = 0.35f;   // 35% of screen width
  static constexpr float height_percent = 0.18f;  // 18% of screen height
  static constexpr float min_width = 300.0f;
  static constexpr float max_width = 600.0f;
  static constexpr float min_height = 100.0f;
  static constexpr float max_height = 200.0f;

  static constexpr float title_margin_x = 10.0f;
  static constexpr float title_margin_y = 10.0f;
  static constexpr int title_font_size = 12;

  static constexpr float text_margin_x = 12.0f;
  static constexpr float text_margin_top = 35.0f;
  static constexpr float text_margin_total_x = 24.0f;
  static constexpr float text_margin_bottom = 45.0f;

  static constexpr float text_font_size = 16.0f;
  static constexpr float text_letter_spacing = 1.5f;
  static constexpr int border_thickness = 1;
};

struct HudConfig {
  // Responsive: use percentages and minimum values
  static constexpr float system_margin_x_percent = 0.015f;  // 1.5% from left
  static constexpr float system_margin_y_percent = 0.015f;  // 1.5% from top
  static constexpr float system_margin_x_min = 10.0f;
  static constexpr float system_margin_y_min = 10.0f;
  static constexpr int system_title_font_size = 15;
  static constexpr float engine_margin_y_offset =
      20.0f;  // Fixed offset from system text
  static constexpr int engine_font_size = 12;

  static constexpr float logs_panel_width_percent =
      0.20f;  // 20% of screen width
  static constexpr float logs_panel_width_min = 180.0f;
  static constexpr float logs_panel_width_max = 300.0f;
  static constexpr float logs_panel_height = 80.0f;
  static constexpr float logs_panel_margin_x_percent =
      0.015f;  // 1.5% from right edge
  static constexpr float logs_panel_margin_x_min = 10.0f;
  static constexpr float logs_title_margin_y = 30.0f;
  static constexpr int logs_font_size = 10;
  static constexpr float logs_line_spacing = 20.0f;
  static constexpr float logs_first_line_y = 50.0f;
  static constexpr float logs_second_line_y = 65.0f;
  static constexpr float logs_third_line_y = 80.0f;
};

struct WaveLayoutConfig {
  static constexpr float vertical_offset_percent = 0.08f;  // 8% from center
  static constexpr float vertical_offset_min = 30.0f;
  static constexpr float vertical_offset_max = 100.0f;
  static constexpr int x_step = 2;
  static constexpr float pulse_min = 0.1f;
  static constexpr float pulse_max = 0.9f;
  static constexpr float phase_offset = PI / 2.0f;  // PI from raymath.h
  static constexpr float fade_alpha = 0.1f;
  static constexpr int max_layers = 3;
  static constexpr int base_radius = 4;
};

struct EmotionTransitionConfig {
  static constexpr float threshold = 0.0001f;
};

struct TextWrapConfig {
  static constexpr int unknown_codepoint = 0x3f;
  static constexpr float line_height_multiplier = 1.5f;
};

struct MemoryConfig {
  static constexpr int rss_field_index = 24;
  static constexpr int fields_before_rss = 23;
  static constexpr float bytes_to_kilobytes = 1024.0f;
  static constexpr float kilobytes_to_megabytes = 1024.0f;
  static constexpr int ram_text_buffer_size = 32;
};

struct LayoutMath {
  static constexpr int center_divisor = 2;
};
}  // namespace LayoutConstants

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
