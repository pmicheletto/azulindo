#include "screen/hud_panel.h"

#include <unistd.h>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>

#include "configs/layout_config.h"
#include "raylib.h"

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

void HudPanel::Draw(int screen_width, int screen_height) const {
  const float system_position_x =
      std::max(static_cast<float>(screen_width) *
                   LayoutConfig::HudConfig::system_margin_x_percent,
               LayoutConfig::HudConfig::system_margin_x_min);
  const float system_position_y =
      std::max(static_cast<float>(screen_height) *
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
      std::clamp(static_cast<float>(screen_width) *
                     LayoutConfig::HudConfig::logs_panel_width_percent,
                 LayoutConfig::HudConfig::logs_panel_width_min,
                 LayoutConfig::HudConfig::logs_panel_width_max);
  const float logs_panel_margin_x =
      std::max(static_cast<float>(screen_width) *
                   LayoutConfig::HudConfig::logs_panel_margin_x_percent,
               LayoutConfig::HudConfig::logs_panel_margin_x_min);
  const float logs_panel_x =
      static_cast<float>(screen_width) - logs_panel_width - logs_panel_margin_x;
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

  char ram_text[LayoutConfig::MemoryConfig::ram_text_buffer_size];
  sprintf(ram_text, "- Memory: %.1f MB", GetCurrentRSSMegabytes());
  DrawText(ram_text, static_cast<int>(logs_text_x),
           static_cast<int>(logs_panel_y +
                            LayoutConfig::HudConfig::logs_third_line_y),
           LayoutConfig::HudConfig::logs_font_size, GREEN);
}
