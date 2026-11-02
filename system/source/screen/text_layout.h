#ifndef AZULINDO_SCREEN_TEXT_LAYOUT_H_
#define AZULINDO_SCREEN_TEXT_LAYOUT_H_

#include "raylib.h"

namespace TextLayout {

float MeasureWrappedContentHeight(Font font, const char *text, float rec_width,
                                  float font_size, float spacing);

void DrawTextWrapped(Font font, const char *text, Rectangle rec,
                     float font_size, float spacing, Color color);

}  // namespace TextLayout

#endif  // AZULINDO_SCREEN_TEXT_LAYOUT_H_
