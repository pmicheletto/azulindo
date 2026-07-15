#include "screen/text_layout.h"

#include "configs/layout_config.h"

namespace TextLayout {

float MeasureWrappedContentHeight(Font font, const char *text, float rec_width,
                                  float font_size, float spacing) {
  int length = TextLength(text);
  float textOffsetY = 0;
  float textOffsetX = 0.0f;
  float scaleFactor = font_size / (float)font.baseSize;

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

void DrawTextWrapped(Font font, const char *text, Rectangle rec,
                     float font_size, float spacing, Color color) {
  int length = TextLength(text);
  float textOffsetY = 0;
  float textOffsetX = 0.0f;
  float scaleFactor = font_size / (float)font.baseSize;

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
                            font_size, color);
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

}  // namespace TextLayout
