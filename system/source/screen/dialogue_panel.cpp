#include "screen/dialogue_panel.h"

#include <algorithm>

#include "configs/layout_config.h"
#include "screen/text_layout.h"

void DialoguePanel::UpdateLayout(int screen_width, int screen_height) {
  const float sw = static_cast<float>(screen_width);
  const float sh = static_cast<float>(screen_height);

  const float d_width =
      std::clamp(sw * LayoutConfig::DialogueConfig::width_percent,
                 LayoutConfig::DialogueConfig::min_width,
                 LayoutConfig::DialogueConfig::max_width);
  const float d_height =
      std::clamp(sh * LayoutConfig::DialogueConfig::height_percent,
                 LayoutConfig::DialogueConfig::min_height,
                 LayoutConfig::DialogueConfig::max_height);

  bounds_ = {
      sw * LayoutConfig::DialogueConfig::position_x_percent,
      sh - (sh * LayoutConfig::DialogueConfig::position_y_offset_percent) -
          d_height,
      d_width, d_height};
}

Rectangle DialoguePanel::TextArea() const {
  return {bounds_.x + LayoutConfig::DialogueConfig::text_margin_x,
          bounds_.y + LayoutConfig::DialogueConfig::text_margin_top,
          bounds_.width - LayoutConfig::DialogueConfig::text_margin_total_x,
          bounds_.height - LayoutConfig::DialogueConfig::text_margin_bottom};
}

void DialoguePanel::Draw(const std::string &text) const {
  DrawRectangleRec(bounds_, LayoutConfig::ColorConfig::dialogue_box);
  DrawRectangleLinesEx(bounds_, LayoutConfig::DialogueConfig::border_thickness,
                       LayoutConfig::ColorConfig::dialogue_border);

  const float title_position_x =
      bounds_.x + LayoutConfig::DialogueConfig::title_margin_x;
  const float title_position_y =
      bounds_.y + LayoutConfig::DialogueConfig::title_margin_y;

  DrawText("AZULINDO:", static_cast<int>(title_position_x),
           static_cast<int>(title_position_y),
           LayoutConfig::DialogueConfig::title_font_size, SKYBLUE);

  if (text.empty()) {
    return;
  }

  TextLayout::DrawTextWrapped(GetFontDefault(), text.c_str(), TextArea(),
                              LayoutConfig::DialogueConfig::text_font_size,
                              LayoutConfig::DialogueConfig::text_letter_spacing,
                              LIGHTGRAY);
}
