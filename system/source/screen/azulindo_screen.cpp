#include "screen/azulindo_screen.h"

#include "core/state_emotion/emotion_manager.h"

namespace {

float GetCurrentRSSMegabytes() {
  std::ifstream stat_stream("/proc/self/stat", std::ios_base::in);
  std::string dummy;
  long rss = 0;

  for (int i = 0; i < LayoutConfig::MemoryConfig::fields_before_rss; ++i) {
    stat_stream >> dummy;
  }
  stat_stream >> rss;

  const float page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024.0f;
  return (static_cast<float>(rss) * page_size_kb) / 1024.0f;
}

}  // namespace

AzulindoScreen::AzulindoScreen(int screen_width, int screen_height) {
  if (screen_width <= 0 || screen_height <= 0) {
    use_full_screen_ = true;
    screen_width_ = GetScreenWidth();
    screen_height_ = GetScreenHeight();
  } else {
    use_full_screen_ = false;
    screen_width_ = screen_width;
    screen_height_ = screen_height;
  }

  wave_config_ = GetEmotionProfile(current_emotion_).wave;
  car_hologram_ = std::make_unique<Hologram>("azulindo.glb");
  UpdateLayout();
  dialogue_text_area_ = DialogueTextArea();
  text_worker_ = std::thread(&AzulindoScreen::TextIngestLoop, this);
}

AzulindoScreen::~AzulindoScreen() {
  {
    std::lock_guard<std::mutex> lock(ingest_mutex_);
    text_worker_running_ = false;
  }
  ingest_cv_.notify_all();
  if (text_worker_.joinable()) {
    text_worker_.join();
  }
}

void AzulindoScreen::UpdateLayout() {
  if (use_full_screen_) {
    screen_width_ = GetScreenWidth();
    screen_height_ = GetScreenHeight();
  }

  const float sw = static_cast<float>(screen_width_);
  const float sh = static_cast<float>(screen_height_);

  const float d_width =
      std::clamp(sw * LayoutConfig::DialogueConfig::width_percent,
                 LayoutConfig::DialogueConfig::min_width,
                 LayoutConfig::DialogueConfig::max_width);
  const float d_height =
      std::clamp(sh * LayoutConfig::DialogueConfig::height_percent,
                 LayoutConfig::DialogueConfig::min_height,
                 LayoutConfig::DialogueConfig::max_height);

  dialogue_bounds_ = {
      sw * LayoutConfig::DialogueConfig::position_x_percent,
      sh - (sh * LayoutConfig::DialogueConfig::position_y_offset_percent) -
          d_height,
      d_width, d_height};
}

void AzulindoScreen::Update(float delta_time) {
  UpdateLayout();
  {
    std::lock_guard<std::mutex> lock(layout_mutex_);
    dialogue_text_area_ = DialogueTextArea();
  }
  if (!pause_wave_while_ai_output_) {
    timer_ += delta_time;
  }
  UpdateEmotion(delta_time);
  car_hologram_->Update(delta_time);
}

void AzulindoScreen::UpdateEmotion(float dt) {
  target_emotion_ = EmotionManager::Instance().GetCurrentState();
  const EmotionProfile &target = GetEmotionProfile(target_emotion_);
  const float factor = dt * target.transition_speed;

  auto update = [factor](float &current, float target_val) {
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

  if (std::abs(wave_config_.wave_density - target.wave.wave_density) < 0.001f) {
    current_emotion_ = target_emotion_;
  }
}

void AzulindoScreen::Draw() {
  ClearBackground(LayoutConfig::ColorConfig::background);
  DrawBackgroundLines();
  DrawWave();
  DrawHud();
  DrawDialogueBox();
  car_hologram_->Draw(wave_config_.wave_color);
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
  EmotionManager::Instance().SetState(emotion);
  timer_ = 0.0f;
}

void AzulindoScreen::AppendAiText(const std::string &text) {
  {
    std::lock_guard<std::mutex> lock(ingest_mutex_);
    text_ingest_queue_.push(text);
  }
  ingest_cv_.notify_one();
}

void AzulindoScreen::ClearAiText() {
  {
    std::lock_guard<std::mutex> lock(ingest_mutex_);
    while (!text_ingest_queue_.empty()) {
      text_ingest_queue_.pop();
    }
  }
  {
    std::lock_guard<std::mutex> lock(text_mutex_);
    ai_response_.clear();
  }
}

void AzulindoScreen::SetPauseWaveWhileAiOutput(bool pause) {
  pause_wave_while_ai_output_ = pause;
}

bool AzulindoScreen::IsAiTextPipelineBusy() const {
  if (text_ingest_active_.load(std::memory_order_acquire)) {
    return true;
  }
  std::lock_guard<std::mutex> lock(ingest_mutex_);
  return !text_ingest_queue_.empty();
}

void AzulindoScreen::TextIngestLoop() {
  while (true) {
    std::string chunk;
    {
      std::unique_lock<std::mutex> lock(ingest_mutex_);
      ingest_cv_.wait(lock, [this] {
        return !text_ingest_queue_.empty() || !text_worker_running_;
      });
      if (!text_worker_running_ && text_ingest_queue_.empty()) {
        break;
      }
      if (text_ingest_queue_.empty()) {
        continue;
      }
      chunk = std::move(text_ingest_queue_.front());
      text_ingest_queue_.pop();
    }

    text_ingest_active_.store(true, std::memory_order_release);

    Rectangle area;
    {
      std::lock_guard<std::mutex> lock(layout_mutex_);
      area = dialogue_text_area_;
    }

    const Font font = GetFontDefault();
    const float font_size = LayoutConfig::DialogueConfig::text_font_size;
    const float spacing = LayoutConfig::DialogueConfig::text_letter_spacing;

    {
      std::lock_guard<std::mutex> lock(text_mutex_);
      std::string candidate = ai_response_ + chunk;
      const float wrapped_height = MeasureWrappedContentHeight(
          font, candidate.c_str(), area.width, font_size, spacing);
      if (wrapped_height > area.height) {
        ai_response_ = std::move(chunk);
      } else {
        ai_response_ = std::move(candidate);
      }
    }

    text_ingest_active_.store(false, std::memory_order_release);
  }
}

void AzulindoScreen::DrawDialogueBox() const {
  DrawRectangleRec(dialogue_bounds_, LayoutConfig::ColorConfig::dialogue_box);
  DrawRectangleLinesEx(dialogue_bounds_,
                       LayoutConfig::DialogueConfig::border_thickness,
                       LayoutConfig::ColorConfig::dialogue_border);

  const float title_position_x =
      dialogue_bounds_.x + LayoutConfig::DialogueConfig::title_margin_x;
  const float title_position_y =
      dialogue_bounds_.y + LayoutConfig::DialogueConfig::title_margin_y;

  DrawText("AZULINDO:", static_cast<int>(title_position_x),
           static_cast<int>(title_position_y),
           LayoutConfig::DialogueConfig::title_font_size, SKYBLUE);

  Rectangle text_area = DialogueTextArea();
  std::string text_copy;
  {
    std::lock_guard<std::mutex> lock(text_mutex_);
    text_copy = ai_response_;
  }

  if (!text_copy.empty()) {
    DrawTextWrapped(GetFontDefault(), text_copy.c_str(), text_area,
                    LayoutConfig::DialogueConfig::text_font_size,
                    LayoutConfig::DialogueConfig::text_letter_spacing,
                    LIGHTGRAY);
  }
}

void AzulindoScreen::DrawHud() const {
  const float system_position_x =
      std::max(static_cast<float>(screen_width_) *
                   LayoutConfig::HudConfig::system_margin_x_percent,
               LayoutConfig::HudConfig::system_margin_x_min);
  const float system_position_y =
      std::max(static_cast<float>(screen_height_) *
                   LayoutConfig::HudConfig::system_margin_y_percent,
               LayoutConfig::HudConfig::system_margin_y_min);
  DrawText("SYSTEM: AZULINDO ACTIVE", static_cast<int>(system_position_x),
           static_cast<int>(system_position_y),
           LayoutConfig::HudConfig::system_title_font_size, SKYBLUE);

  const float engine_position_y =
      system_position_y + LayoutConfig::HudConfig::engine_margin_y_offset;
  DrawText("ENGINE: ONLINE", static_cast<int>(system_position_x),
           static_cast<int>(engine_position_y),
           LayoutConfig::HudConfig::engine_font_size, BLUE);

  const float logs_panel_width =
      std::clamp(static_cast<float>(screen_width_) *
                     LayoutConfig::HudConfig::logs_panel_width_percent,
                 LayoutConfig::HudConfig::logs_panel_width_min,
                 LayoutConfig::HudConfig::logs_panel_width_max);
  const float logs_panel_margin_x =
      std::max(static_cast<float>(screen_width_) *
                   LayoutConfig::HudConfig::logs_panel_margin_x_percent,
               LayoutConfig::HudConfig::logs_panel_margin_x_min);
  const float logs_panel_x = static_cast<float>(screen_width_) -
                             logs_panel_width - logs_panel_margin_x;
  const float logs_panel_y = system_position_y;
  DrawRectangle(static_cast<int>(logs_panel_x), static_cast<int>(logs_panel_y),
                static_cast<int>(logs_panel_width),
                static_cast<int>(LayoutConfig::HudConfig::logs_panel_height),
                LayoutConfig::ColorConfig::hud_panel);

  const float logs_text_x = logs_panel_x + 10.0f;
  DrawText("AZULINDO LOGS:", static_cast<int>(logs_text_x),
           static_cast<int>(logs_panel_y +
                            LayoutConfig::HudConfig::logs_title_margin_y),
           LayoutConfig::HudConfig::logs_font_size, SKYBLUE);

  DrawText("- Core: OK", static_cast<int>(logs_text_x),
           static_cast<int>(logs_panel_y +
                            LayoutConfig::HudConfig::logs_first_line_y),
           LayoutConfig::HudConfig::logs_font_size, LIGHTGRAY);
  DrawText("- Wave Engine: STABLE", static_cast<int>(logs_text_x),
           static_cast<int>(logs_panel_y +
                            LayoutConfig::HudConfig::logs_second_line_y),
           LayoutConfig::HudConfig::logs_font_size, LIGHTGRAY);

  char ramText[LayoutConfig::MemoryConfig::ram_text_buffer_size];
  sprintf(ramText, "- Memory: %.1f MB", GetCurrentRSSMegabytes());
  DrawText(ramText, static_cast<int>(logs_text_x),
           static_cast<int>(logs_panel_y +
                            LayoutConfig::HudConfig::logs_third_line_y),
           LayoutConfig::HudConfig::logs_font_size, GREEN);
}
void AzulindoScreen::DrawWave() const {
  const float vertical_offset =
      std::clamp(static_cast<float>(screen_height_) *
                     LayoutConfig::WaveLayoutConfig::vertical_offset_percent,
                 LayoutConfig::WaveLayoutConfig::vertical_offset_min,
                 LayoutConfig::WaveLayoutConfig::vertical_offset_max);
  const int wave_position_y =
      (screen_height_ / 2) - static_cast<int>(vertical_offset);
  const int wave_position_x = (screen_width_ / 2);

  const float pulse_min = LayoutConfig::WaveLayoutConfig::pulse_min;
  const float pulse_max = LayoutConfig::WaveLayoutConfig::pulse_max;
  const float pulse =
      sinf(timer_ * wave_config_.animation_speed) * pulse_min + pulse_max;

  const float amplitude =
      wave_config_.amplitude_min +
      (wave_config_.amplitude_max - wave_config_.amplitude_min) * pulse;

  const int x_step = LayoutConfig::WaveLayoutConfig::x_step;
  for (int x = 0; x < screen_width_; x += x_step) {
    float h1 = sinf(x * wave_config_.wave_density +
                    timer_ * wave_config_.animation_speed) *
               amplitude;
    float h2 = sinf(x * wave_config_.wave_density +
                    timer_ * wave_config_.animation_speed +
                    LayoutConfig::WaveLayoutConfig::phase_offset) *
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
                  LayoutConfig::WaveLayoutConfig::fade_alpha));
  }
}

float AzulindoScreen::MeasureWrappedContentHeight(Font font, const char *text,
                                                  float rec_width,
                                                  float fontSize,
                                                  float spacing) const {
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

    if (codepoint == LayoutConfig::TextWrapConfig::unknown_codepoint)
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

      if ((textOffsetX + glyphWidth) > rec_width) {
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
    } else {
      if (i == endLine) {
        const float line_height =
            font.baseSize *
            LayoutConfig::TextWrapConfig::line_height_multiplier;
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
  return textOffsetY;
}

Rectangle AzulindoScreen::DialogueTextArea() const {
  return {dialogue_bounds_.x + LayoutConfig::DialogueConfig::text_margin_x,
          dialogue_bounds_.y + LayoutConfig::DialogueConfig::text_margin_top,
          dialogue_bounds_.width -
              LayoutConfig::DialogueConfig::text_margin_total_x,
          dialogue_bounds_.height -
              LayoutConfig::DialogueConfig::text_margin_bottom};
}

void AzulindoScreen::DrawTextWrapped(Font font, const char *text, Rectangle rec,
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

    if (codepoint == LayoutConfig::TextWrapConfig::unknown_codepoint)
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
    } else {
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
            LayoutConfig::TextWrapConfig::line_height_multiplier;
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