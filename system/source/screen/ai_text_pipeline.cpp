#include "screen/ai_text_pipeline.h"

#include "configs/layout_config.h"
#include "raylib.h"
#include "screen/dialogue_panel.h"
#include "screen/text_layout.h"

AiTextPipeline::AiTextPipeline(DialoguePanel &dialogue) : dialogue_(dialogue) {}

void AiTextPipeline::Append(const std::string &text) {
  if (text.empty()) {
    return;
  }

  const Rectangle area = dialogue_.TextArea();
  const Font font = GetFontDefault();
  const float font_size = LayoutConfig::DialogueConfig::text_font_size;
  const float spacing = LayoutConfig::DialogueConfig::text_letter_spacing;

  std::string candidate = ai_response_ + text;
  const float wrapped_height = TextLayout::MeasureWrappedContentHeight(
      font, candidate.c_str(), area.width, font_size, spacing);
  if (wrapped_height > area.height) {
    ai_response_ = text;
  } else {
    ai_response_ = std::move(candidate);
  }
}

void AiTextPipeline::Clear() { ai_response_.clear(); }
