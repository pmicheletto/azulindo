#ifndef AZULINDO_APP_UTF8_INPUT_H_
#define AZULINDO_APP_UTF8_INPUT_H_

#include <string>

namespace Utf8Input {

void AppendCodepoint(std::string *s, int codepoint);
void PopLastChar(std::string *s);

// Maps fancy punctuation and Latin accents to plain ASCII.
// Returns empty to drop unsupported glyphs (emoji, etc.).
std::string AsciiFallback(int codepoint);

// Rewrites text so the default raylib font can draw it (no '?').
// Accents become base letters (mao, Sao); emojis are removed.
std::string SanitizeForHudFont(const std::string &utf8);

}  // namespace Utf8Input

#endif  // AZULINDO_APP_UTF8_INPUT_H_
