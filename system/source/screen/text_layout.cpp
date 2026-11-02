#include "screen/text_layout.h"

#include "configs/layout_config.h"

namespace TextLayout {
namespace {

enum class WrapPass { kMeasureOnly, kDraw };

float ProcessWrapped(Font font, const char *text, Rectangle rec,
                     float font_size, float spacing, Color color,
                     WrapPass pass) {
  const int length = TextLength(text);
  float text_offset_y = 0.0f;
  float text_offset_x = 0.0f;
  const float scale_factor = font_size / static_cast<float>(font.baseSize);

  enum { kMeasureState = 0, kDrawState = 1 };
  int state = kMeasureState;
  int start_line = -1;
  int end_line = -1;

  for (int i = 0; i < length; i++) {
    int codepoint_byte_count = 0;
    int codepoint = GetCodepoint(&text[i], &codepoint_byte_count);
    const int index = GetGlyphIndex(font, codepoint);

    if (codepoint == LayoutConfig::TextWrapConfig::unknown_codepoint) {
      codepoint_byte_count = 1;
    }
    i += (codepoint_byte_count - 1);

    float glyph_width = 0.0f;
    if (codepoint != '\n') {
      glyph_width = (font.glyphs[index].advanceX == 0)
                        ? font.recs[index].width * scale_factor
                        : font.glyphs[index].advanceX * scale_factor;
      if (i + 1 < length) {
        glyph_width += spacing;
      }
    }

    if (state == kMeasureState) {
      if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n')) {
        end_line = i;
      }

      if ((text_offset_x + glyph_width) > rec.width) {
        end_line = (end_line < 1) ? i : end_line;
        if (i == end_line) {
          end_line -= codepoint_byte_count;
        }
        if ((start_line + codepoint_byte_count) == end_line) {
          end_line = (i - codepoint_byte_count);
        }
        state = kDrawState;
      } else if ((i + 1) == length) {
        end_line = i;
        state = kDrawState;
      } else if (codepoint == '\n') {
        state = kDrawState;
      }

      if (state == kDrawState) {
        text_offset_x = 0.0f;
        i = start_line;
        glyph_width = 0.0f;
      }
    } else {
      if (pass == WrapPass::kDraw && codepoint != '\n') {
        if ((text_offset_y + font.baseSize * scale_factor) > rec.height) {
          break;
        }
        if ((codepoint != ' ') && (codepoint != '\t')) {
          DrawTextCodepoint(font, codepoint,
                            (Vector2){rec.x + text_offset_x,
                                      rec.y + text_offset_y},
                            font_size, color);
        }
      }

      if (i == end_line) {
        const float line_height =
            font.baseSize *
            LayoutConfig::TextWrapConfig::line_height_multiplier;
        text_offset_y += line_height * scale_factor;
        text_offset_x = 0.0f;
        start_line = end_line;
        end_line = -1;
        glyph_width = 0.0f;
        state = kMeasureState;
      }
    }

    if ((text_offset_x != 0.0f) || (codepoint != ' ')) {
      text_offset_x += glyph_width;
    }
  }
  return text_offset_y;
}

}  // namespace

float MeasureWrappedContentHeight(Font font, const char *text, float rec_width,
                                  float font_size, float spacing) {
  const Rectangle measure_rect = {0.0f, 0.0f, rec_width, 1.0e9f};
  return ProcessWrapped(font, text, measure_rect, font_size, spacing, BLANK,
                        WrapPass::kMeasureOnly);
}

void DrawTextWrapped(Font font, const char *text, Rectangle rec,
                     float font_size, float spacing, Color color) {
  ProcessWrapped(font, text, rec, font_size, spacing, color, WrapPass::kDraw);
}

}  // namespace TextLayout
