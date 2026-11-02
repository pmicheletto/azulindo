#include "screen/hud_panel.h"

#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>

#include "configs/layout_config.h"
#include "raylib.h"

namespace {

struct HudStatusCopy {
  const char *system_line;
  const char *engine_line;
  const char *core_line;
  Color system_color;
  Color engine_color;
  Color core_color;
};

HudStatusCopy ResolveStatusCopy(SystemStatus status) {
  using Hud = LayoutConfig::HudConfig;
  switch (status) {
    case SystemStatus::kBooting:
      return {Hud::kSystemLoading, Hud::kEngineBooting, Hud::kCoreLoading,
              ORANGE, ORANGE, ORANGE};
    case SystemStatus::kOffline:
      return {Hud::kSystemOffline, Hud::kEngineOffline, Hud::kCoreOffline, RED,
              RED, RED};
    case SystemStatus::kOnline:
      return {Hud::kSystemActive, Hud::kEngineOnline, Hud::kCoreOnline, SKYBLUE,
              BLUE, LIGHTGRAY};
  }
  return {Hud::kSystemOffline, Hud::kEngineOffline, Hud::kCoreOffline, RED, RED,
          RED};
}

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

const char *ResolveLlmLine(SystemStatus status, const std::string &llm_model,
                           char *buffer, std::size_t buffer_size) {
  using Hud = LayoutConfig::HudConfig;
  switch (status) {
    case SystemStatus::kBooting:
      return Hud::kLlmLoading;
    case SystemStatus::kOffline:
      return Hud::kLlmOffline;
    case SystemStatus::kOnline:
      std::snprintf(buffer, buffer_size, Hud::kLlmFormat, llm_model.c_str());
      return buffer;
  }
  return Hud::kLlmOffline;
}

}  // namespace

void HudPanel::Draw(int screen_width, int screen_height, SystemStatus status,
                    const std::string &llm_model) const {
  const float origin_x =
      std::max(static_cast<float>(screen_width) *
                   LayoutConfig::HudConfig::system_margin_x_percent,
               LayoutConfig::HudConfig::system_margin_x_min);
  const float origin_y =
      std::max(static_cast<float>(screen_height) *
                   LayoutConfig::HudConfig::system_margin_y_percent,
               LayoutConfig::HudConfig::system_margin_y_min);

  DrawSystemBanner(origin_x, origin_y, status);
  DrawLogsPanel(origin_y, screen_width, status, llm_model);
}

void HudPanel::DrawSystemBanner(float origin_x, float origin_y,
                                SystemStatus status) const {
  const HudStatusCopy copy = ResolveStatusCopy(status);

  DrawText(copy.system_line, static_cast<int>(origin_x),
           static_cast<int>(origin_y),
           LayoutConfig::HudConfig::system_title_font_size, copy.system_color);

  const float engine_y =
      origin_y + LayoutConfig::HudConfig::engine_margin_y_offset;
  DrawText(copy.engine_line, static_cast<int>(origin_x),
           static_cast<int>(engine_y), LayoutConfig::HudConfig::engine_font_size,
           copy.engine_color);
}

void HudPanel::DrawLogsPanel(float origin_y, int screen_width,
                             SystemStatus status,
                             const std::string &llm_model) const {
  const HudStatusCopy copy = ResolveStatusCopy(status);

  const float panel_width =
      std::clamp(static_cast<float>(screen_width) *
                     LayoutConfig::HudConfig::logs_panel_width_percent,
                 LayoutConfig::HudConfig::logs_panel_width_min,
                 LayoutConfig::HudConfig::logs_panel_width_max);
  const float panel_margin_x =
      std::max(static_cast<float>(screen_width) *
                   LayoutConfig::HudConfig::logs_panel_margin_x_percent,
               LayoutConfig::HudConfig::logs_panel_margin_x_min);
  const float panel_x =
      static_cast<float>(screen_width) - panel_width - panel_margin_x;

  DrawRectangle(static_cast<int>(panel_x), static_cast<int>(origin_y),
                static_cast<int>(panel_width),
                static_cast<int>(LayoutConfig::HudConfig::logs_panel_height),
                LayoutConfig::ColorConfig::hud_panel);

  const float text_x =
      panel_x + LayoutConfig::HudConfig::logs_panel_padding_x;
  const int font_size = LayoutConfig::HudConfig::logs_font_size;

  DrawText(LayoutConfig::HudConfig::kLogsTitle, static_cast<int>(text_x),
           static_cast<int>(origin_y +
                            LayoutConfig::HudConfig::logs_title_margin_y),
           font_size, SKYBLUE);
  DrawText(copy.core_line, static_cast<int>(text_x),
           static_cast<int>(origin_y +
                            LayoutConfig::HudConfig::logs_first_line_y),
           font_size, copy.core_color);

  const double now = GetTime();
  if (last_rss_sample_time_ < 0.0 ||
      (now - last_rss_sample_time_) >=
          LayoutConfig::MemoryConfig::rss_sample_interval_seconds) {
    cached_rss_mb_ = GetCurrentRSSMegabytes();
    last_rss_sample_time_ = now;
  }

  char ram_text[LayoutConfig::MemoryConfig::ram_text_buffer_size];
  std::snprintf(ram_text, sizeof(ram_text),
                LayoutConfig::HudConfig::kSystemRamFormat, cached_rss_mb_);
  DrawText(ram_text, static_cast<int>(text_x),
           static_cast<int>(origin_y +
                            LayoutConfig::HudConfig::logs_second_line_y),
           font_size, GREEN);

  char llm_text[LayoutConfig::MemoryConfig::llm_text_buffer_size];
  const char *llm_line =
      ResolveLlmLine(status, llm_model, llm_text, sizeof(llm_text));
  DrawText(llm_line, static_cast<int>(text_x),
           static_cast<int>(origin_y +
                            LayoutConfig::HudConfig::logs_third_line_y),
           font_size, LIGHTGRAY);
}
