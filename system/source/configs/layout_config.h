#ifndef AZULINDO_CONFIGS_LAYOUT_CONFIG_H_
#define AZULINDO_CONFIGS_LAYOUT_CONFIG_H_

#include "raylib.h"
#include "raymath.h"

class LayoutConfig {
 public:
  struct BackgroundConfig {
    static constexpr int line_spacing = 5; // lines of text in the background
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
    static constexpr float position_x_percent = 0.03f;
    static constexpr float position_y_offset_percent = 0.15f;
    static constexpr float width_percent = 0.48f;
    static constexpr float height_percent = 0.32f;
    static constexpr float min_width = 360.0f;
    static constexpr float max_width = 820.0f;
    static constexpr float min_height = 160.0f;
    static constexpr float max_height = 360.0f;

    static constexpr float title_margin_x = 10.0f;
    static constexpr float title_margin_y = 10.0f;
    static constexpr int title_font_size = 12;

    static constexpr float text_margin_x = 12.0f;
    static constexpr float text_margin_top = 32.0f;
    static constexpr float text_margin_total_x = 24.0f;
    // Smaller bottom margin = more room before the speech box resets.
    static constexpr float text_margin_bottom = 18.0f;

    static constexpr float text_font_size = 16.0f;
    static constexpr float text_letter_spacing = 1.5f;
    static constexpr int border_thickness = 1;
  };

  struct HudConfig {
    static constexpr float system_margin_x_percent = 0.015f;
    static constexpr float system_margin_y_percent = 0.015f;
    static constexpr float system_margin_x_min = 10.0f;
    static constexpr float system_margin_y_min = 10.0f;
    static constexpr int system_title_font_size = 15;
    static constexpr float engine_margin_y_offset = 20.0f;
    static constexpr int engine_font_size = 12;

    static constexpr const char *kSystemActive = "SYSTEM: AZULINDO ACTIVE";
    static constexpr const char *kSystemLoading = "SYSTEM: LOADING LLM MODEL";
    static constexpr const char *kSystemOffline = "SYSTEM: LLM OFFLINE";
    static constexpr const char *kEngineOnline = "ENGINE: ONLINE";
    static constexpr const char *kEngineBooting = "ENGINE: BOOTING";
    static constexpr const char *kEngineOffline = "ENGINE: OFFLINE";
    static constexpr const char *kLogsTitle = "AZULINDO LOGS:";
    static constexpr const char *kCoreOnline = "- Core: OK";
    static constexpr const char *kCoreLoading = "- Core: LOADING LLM";
    static constexpr const char *kCoreOffline = "- Core: LLM OFFLINE";
    static constexpr const char *kSystemRamFormat = "- System RAM: %.1f MB";
    static constexpr const char *kLlmFormat = "- LLM: %s";
    static constexpr const char *kLlmLoading = "- LLM: LOADING";
    static constexpr const char *kLlmOffline = "- LLM: OFFLINE";

    static constexpr float logs_panel_width_percent = 0.20f;
    static constexpr float logs_panel_width_min = 180.0f;
    static constexpr float logs_panel_width_max = 300.0f;
    static constexpr float logs_panel_height = 80.0f;
    static constexpr float logs_panel_margin_x_percent = 0.015f;
    static constexpr float logs_panel_margin_x_min = 10.0f;
    static constexpr float logs_panel_padding_x = 10.0f;
    static constexpr float logs_title_margin_y = 10.0f;
    static constexpr int logs_font_size = 10;
    static constexpr float logs_first_line_y = 25.0f;
    static constexpr float logs_second_line_y = 40.0f;
    static constexpr float logs_third_line_y = 55.0f;
  };

  struct LanguageSelectorConfig {
    static constexpr int font_size = 14;
    static constexpr float top_margin = 10.0f;
    static constexpr float hit_padding_x = 8.0f;
    static constexpr float hit_padding_y = 4.0f;
    static constexpr float separator_gap = 6.0f;
    static constexpr const char *kLabelBr = "BR";
    static constexpr const char *kLabelSeparator = "|";
    static constexpr const char *kLabelEn = "EN";
  };

  struct WaveLayoutConfig {
    // Larger offset = wave higher on screen (y = center - offset).
    static constexpr float vertical_offset_percent = 0.18f;
    static constexpr float vertical_offset_min = 50.0f;
    static constexpr float vertical_offset_max = 160.0f;
    static constexpr int x_step = 3; // fewer DrawLine calls (important on software GL). - maybe 8
    static constexpr float pulse_min = 0.1f;
    static constexpr float pulse_max = 0.9f;
    static constexpr float phase_offset = PI / 2.0f;
    static constexpr float fade_alpha = 0.1f;
  };

  struct EmotionTransitionConfig {
    static constexpr float kDensityThreshold = 0.001f;
  };

  struct TextWrapConfig {
    static constexpr int unknown_codepoint = 0x3f;
    static constexpr float line_height_multiplier = 1.5f;
  };

  struct MemoryConfig {
    static constexpr int fields_before_rss = 23;
    static constexpr int ram_text_buffer_size = 40;
    static constexpr int llm_text_buffer_size = 64;
    static constexpr double rss_sample_interval_seconds = 0.5;
  };

  struct HologramConfig {
    static constexpr const char *kModelFilename = "azulindo.glb";

    struct CameraConfig {
      static constexpr float position_x = 0.0f;
      static constexpr float position_y = 0.5f;
      static constexpr float position_z = 2.0f;
      static constexpr float target_x = 0.0f;
      static constexpr float target_y = 0.0f;
      static constexpr float target_z = 0.0f;
      static constexpr float up_x = 0.0f;
      static constexpr float up_y = 1.0f;
      static constexpr float up_z = 0.0f;
      static constexpr float fovy = 45.0f;
    };

    struct ModelConfig {
      static constexpr float position_x = 0.6f;
      static constexpr float position_y = -0.26f;
      static constexpr float position_z = 0.4f;
      static constexpr float rotation_axis_x = 0.0f;
      static constexpr float rotation_axis_y = 1.0f;
      static constexpr float rotation_axis_z = 0.0f;
      static constexpr float scale_x = 0.0025f;
      static constexpr float scale_y = 0.0025f;
      static constexpr float scale_z = 0.0025f;
    };

    struct AnimationConfig {
      static constexpr float rotation_speed_degrees_per_second = 45.0f;
      static constexpr float color_fade_alpha = 0.5f;
    };
  };
};

#endif  // AZULINDO_CONFIGS_LAYOUT_CONFIG_H_
