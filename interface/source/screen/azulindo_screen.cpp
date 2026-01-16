#include "screen/azulindo_screen.h"

namespace {
float GetCurrentRSS() {
  std::ifstream stat_stream("/proc/self/stat", std::ios_base::in);
  std::string dummy;
  long rss;

  for (int i = 0; i < LayoutConstants::MemoryConfig::fields_before_rss; ++i)
    stat_stream >> dummy;
  stat_stream >> rss;

  float page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024.0f;
  return (rss * page_size_kb) / 1024.0f;  // Retorna em MB
}
}  // namespace

AzulindoScreen::AzulindoScreen(int screen_width, int screen_height)
    : screen_width_(screen_width), screen_height_(screen_height) {
  wave_config_ = GetEmotionProfile(current_emotion_).wave;
  car_hologram_ = std::make_unique<Hologram>("azulindo.glb");

  // Calculate responsive dialogue box dimensions
  const float dialogue_width =
      std::clamp(static_cast<float>(screen_width) *
                     LayoutConstants::DialogueConfig::width_percent,
                 LayoutConstants::DialogueConfig::min_width,
                 LayoutConstants::DialogueConfig::max_width);
  const float dialogue_height =
      std::clamp(static_cast<float>(screen_height) *
                     LayoutConstants::DialogueConfig::height_percent,
                 LayoutConstants::DialogueConfig::min_height,
                 LayoutConstants::DialogueConfig::max_height);
  const float dialogue_position_x =
      static_cast<float>(screen_width) *
      LayoutConstants::DialogueConfig::position_x_percent;
  const float dialogue_position_y =
      static_cast<float>(screen_height) -
      (static_cast<float>(screen_height) *
       LayoutConstants::DialogueConfig::position_y_offset_percent) -
      dialogue_height;

  dialogue_bounds_ = {dialogue_position_x, dialogue_position_y, dialogue_width,
                      dialogue_height};
}

void AzulindoScreen::SetEmotion(EmotionState emotion) {
  target_emotion_ = emotion;
  timer_ = 0.0f;
}

void AzulindoScreen::Update(float delta_time) {
  timer_ += delta_time;
  UpdateEmotion(delta_time);
  car_hologram_->Update(delta_time);
}

void AzulindoScreen::Draw() {
  ClearBackground(LayoutConstants::ColorConfig::background);

  DrawBackgroundLines();
  DrawWave();
  DrawHud();
  DrawDialogueBox();
  car_hologram_->Draw(wave_config_.wave_color);
}

void AzulindoScreen::UpdateEmotion(float dt) {
  const EmotionProfile& target = GetEmotionProfile(target_emotion_);
  const float factor = dt * target.transition_speed;

  auto update = [factor](float& current, float target_val) {
    current = Lerp(current, target_val, factor);
  };

  update(wave_config_.wave_density, target.wave.wave_density);
  update(wave_config_.amplitude_min, target.wave.amplitude_min);
  update(wave_config_.amplitude_max, target.wave.amplitude_max);
  update(wave_config_.harmonic_strength, target.wave.harmonic_strength);
  update(wave_config_.harmonic_frequency, target.wave.harmonic_frequency);
  update(wave_config_.animation_speed, target.wave.animation_speed);
  update(wave_config_.harmonic_speed, target.wave.harmonic_speed);

  wave_config_.wave_color =
      ColorLerp(wave_config_.wave_color, target.wave.wave_color, factor);
  wave_config_.glow_color =
      ColorLerp(wave_config_.glow_color, target.wave.glow_color, factor);

  const float threshold = LayoutConstants::EmotionTransitionConfig::threshold;
  if (std::abs(wave_config_.wave_density - target.wave.wave_density) <
          threshold &&
      std::abs(wave_config_.animation_speed - target.wave.animation_speed) <
          threshold) {
    current_emotion_ = target_emotion_;
  }
}

void AzulindoScreen::DrawBackgroundLines() const {
  const int line_spacing = LayoutConstants::BackgroundConfig::line_spacing;
  const int line_thickness = LayoutConstants::BackgroundConfig::line_thickness;
  const Color line_color = LayoutConstants::ColorConfig::background_line;

  for (int y = 0; y < screen_height_; y += line_spacing) {
    DrawRectangle(0, y, screen_width_, line_thickness, line_color);
  }
}

void AzulindoScreen::DrawDialogueBox() const {
  DrawRectangleRec(dialogue_bounds_,
                   LayoutConstants::ColorConfig::dialogue_box);

  DrawRectangleLinesEx(dialogue_bounds_,
                       LayoutConstants::DialogueConfig::border_thickness,
                       LayoutConstants::ColorConfig::dialogue_border);

  const float title_position_x =
      dialogue_bounds_.x + LayoutConstants::DialogueConfig::title_margin_x;
  const float title_position_y =
      dialogue_bounds_.y + LayoutConstants::DialogueConfig::title_margin_y;
  DrawText("AZULINDO:", static_cast<int>(title_position_x),
           static_cast<int>(title_position_y),
           LayoutConstants::DialogueConfig::title_font_size, SKYBLUE);

  const float text_area_x =
      dialogue_bounds_.x + LayoutConstants::DialogueConfig::text_margin_x;
  const float text_area_y =
      dialogue_bounds_.y + LayoutConstants::DialogueConfig::text_margin_top;
  const float text_area_width =
      dialogue_bounds_.width -
      LayoutConstants::DialogueConfig::text_margin_total_x;
  const float text_area_height =
      dialogue_bounds_.height -
      LayoutConstants::DialogueConfig::text_margin_bottom;

  Rectangle text_area = {text_area_x, text_area_y, text_area_width,
                         text_area_height};

  DrawTextWrapped(GetFontDefault(), ai_text_, text_area,
                  LayoutConstants::DialogueConfig::text_font_size,
                  LayoutConstants::DialogueConfig::text_letter_spacing,
                  LIGHTGRAY);
}

void AzulindoScreen::DrawHud() const {
  const float system_position_x =
      std::max(static_cast<float>(screen_width_) *
                   LayoutConstants::HudConfig::system_margin_x_percent,
               LayoutConstants::HudConfig::system_margin_x_min);
  const float system_position_y =
      std::max(static_cast<float>(screen_height_) *
                   LayoutConstants::HudConfig::system_margin_y_percent,
               LayoutConstants::HudConfig::system_margin_y_min);
  DrawText("SYSTEM: AZULINDO ACTIVE", static_cast<int>(system_position_x),
           static_cast<int>(system_position_y),
           LayoutConstants::HudConfig::system_title_font_size, SKYBLUE);

  const float engine_position_y =
      system_position_y + LayoutConstants::HudConfig::engine_margin_y_offset;
  DrawText("ENGINE: ONLINE", static_cast<int>(system_position_x),
           static_cast<int>(engine_position_y),
           LayoutConstants::HudConfig::engine_font_size, BLUE);

  const float logs_panel_width =
      std::clamp(static_cast<float>(screen_width_) *
                     LayoutConstants::HudConfig::logs_panel_width_percent,
                 LayoutConstants::HudConfig::logs_panel_width_min,
                 LayoutConstants::HudConfig::logs_panel_width_max);
  const float logs_panel_margin_x =
      std::max(static_cast<float>(screen_width_) *
                   LayoutConstants::HudConfig::logs_panel_margin_x_percent,
               LayoutConstants::HudConfig::logs_panel_margin_x_min);
  const float logs_panel_x = static_cast<float>(screen_width_) -
                             logs_panel_width - logs_panel_margin_x;
  const float logs_panel_y = system_position_y;
  DrawRectangle(static_cast<int>(logs_panel_x), static_cast<int>(logs_panel_y),
                static_cast<int>(logs_panel_width),
                static_cast<int>(LayoutConstants::HudConfig::logs_panel_height),
                LayoutConstants::ColorConfig::hud_panel);

  const float logs_text_x = logs_panel_x + 10.0f;  // Small padding inside panel
  DrawText("AZULINDO LOGS:", static_cast<int>(logs_text_x),
           static_cast<int>(logs_panel_y +
                            LayoutConstants::HudConfig::logs_title_margin_y),
           LayoutConstants::HudConfig::logs_font_size, SKYBLUE);

  DrawText("- Core: OK", static_cast<int>(logs_text_x),
           static_cast<int>(logs_panel_y +
                            LayoutConstants::HudConfig::logs_first_line_y),
           LayoutConstants::HudConfig::logs_font_size, LIGHTGRAY);
  DrawText("- Wave Engine: STABLE", static_cast<int>(logs_text_x),
           static_cast<int>(logs_panel_y +
                            LayoutConstants::HudConfig::logs_second_line_y),
           LayoutConstants::HudConfig::logs_font_size, LIGHTGRAY);

  char ramText[LayoutConstants::MemoryConfig::ram_text_buffer_size];
  sprintf(ramText, "- Memory: %.1f MB", GetCurrentRSS());
  DrawText(ramText, static_cast<int>(logs_text_x),
           static_cast<int>(logs_panel_y +
                            LayoutConstants::HudConfig::logs_third_line_y),
           LayoutConstants::HudConfig::logs_font_size, GREEN);
}
void AzulindoScreen::DrawWave() const {
  const float vertical_offset =
      std::clamp(static_cast<float>(screen_height_) *
                     LayoutConstants::WaveLayoutConfig::vertical_offset_percent,
                 LayoutConstants::WaveLayoutConfig::vertical_offset_min,
                 LayoutConstants::WaveLayoutConfig::vertical_offset_max);
  const int wave_position_y =
      (screen_height_ / LayoutConstants::LayoutMath::center_divisor) -
      static_cast<int>(vertical_offset);
  const int wave_position_x =
      screen_width_ / LayoutConstants::LayoutMath::center_divisor;

  const float pulse_min = LayoutConstants::WaveLayoutConfig::pulse_min;
  const float pulse_max = LayoutConstants::WaveLayoutConfig::pulse_max;
  const float pulse =
      sinf(timer_ * wave_config_.animation_speed) * pulse_min + pulse_max;

  const float amplitude =
      wave_config_.amplitude_min +
      (wave_config_.amplitude_max - wave_config_.amplitude_min) * pulse;

  const int x_step = LayoutConstants::WaveLayoutConfig::x_step;
  for (int x = 0; x < screen_width_; x += x_step) {
    float h1 = sinf(x * wave_config_.wave_density +
                    timer_ * wave_config_.animation_speed) *
               amplitude;
    float h2 = sinf(x * wave_config_.wave_density +
                    timer_ * wave_config_.animation_speed +
                    LayoutConstants::WaveLayoutConfig::phase_offset) *
               amplitude;

    float harmonic = sinf(x * wave_config_.harmonic_frequency -
                          timer_ * wave_config_.harmonic_speed) *
                     (amplitude * wave_config_.harmonic_strength);
    h1 += harmonic;
    h2 -= harmonic;

    int pos_y1 = wave_position_y + static_cast<int>(h1);
    int pos_y2 = wave_position_y + static_cast<int>(h2);
    int pos_x1 = wave_position_x + static_cast<int>(h2);

    DrawLine(pos_x1, pos_y1, x, pos_y2,
             Fade(wave_config_.wave_color,
                  LayoutConstants::WaveLayoutConfig::fade_alpha));

    const int max_layers = LayoutConstants::WaveLayoutConfig::max_layers;
    const int base_radius = LayoutConstants::WaveLayoutConfig::base_radius;
    for (int layer = 1; layer <= max_layers; ++layer) {
      float radius = static_cast<float>(base_radius - layer);
      Color layer_color =
          Fade(wave_config_.glow_color,
               LayoutConstants::WaveLayoutConfig::fade_alpha / layer);

      // DrawCircle(x, pos_y1, radius, layer_color);
      // DrawCircle(x, pos_y2, radius, layer_color);
    }
  }
}
// method from raylib examples
void AzulindoScreen::DrawTextWrapped(Font font, const char* text, Rectangle rec,
                                     float fontSize, float spacing,
                                     Color color) const {
  int length = TextLength(text);
  float textOffsetY = 0;
  float textOffsetX = 0.0f;
  float scaleFactor = fontSize / (float)font.baseSize;

  enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
  int state = MEASURE_STATE;

  int startLine = -1;
  int endLine = -1;

  for (int i = 0; i < length; i++) {
    int codepointByteCount = 0;
    int codepoint = GetCodepoint(&text[i], &codepointByteCount);
    int index = GetGlyphIndex(font, codepoint);

    if (codepoint == LayoutConstants::TextWrapConfig::unknown_codepoint)
      codepointByteCount = 1;
    i += (codepointByteCount - 1);

    float glyphWidth = 0;
    if (codepoint != '\n') {
      glyphWidth = (font.glyphs[index].advanceX == 0)
                       ? font.recs[index].width * scaleFactor
                       : font.glyphs[index].advanceX * scaleFactor;
      if (i + 1 < length) glyphWidth += spacing;
    }
    if (state == MEASURE_STATE) {
      if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n'))
        endLine = i;

      if ((textOffsetX + glyphWidth) > rec.width) {
        endLine = (endLine < 1) ? i : endLine;
        if (i == endLine) endLine -= codepointByteCount;
        if ((startLine + codepointByteCount) == endLine)
          endLine = (i - codepointByteCount);
        state = DRAW_STATE;
      } else if ((i + 1) == length) {
        endLine = i;
        state = DRAW_STATE;
      } else if (codepoint == '\n') {
        state = DRAW_STATE;
      }

      if (state == DRAW_STATE) {
        textOffsetX = 0;
        i = startLine;
        glyphWidth = 0;
      }
    } else {  // DRAW_STATE
      if (codepoint != '\n') {
        if ((textOffsetY + font.baseSize * scaleFactor) > rec.height) break;

        if ((codepoint != ' ') && (codepoint != '\t')) {
          DrawTextCodepoint(font, codepoint,
                            (Vector2){rec.x + textOffsetX, rec.y + textOffsetY},
                            fontSize, color);
        }
      }

      if (i == endLine) {
        const float line_height =
            font.baseSize *
            LayoutConstants::TextWrapConfig::line_height_multiplier;
        textOffsetY += line_height * scaleFactor;
        textOffsetX = 0;
        startLine = endLine;
        endLine = -1;
        glyphWidth = 0;
        state = MEASURE_STATE;
      }
    }
    if ((textOffsetX != 0) || (codepoint != ' ')) textOffsetX += glyphWidth;
  }
}