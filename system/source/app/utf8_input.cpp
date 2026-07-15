#include "app/utf8_input.h"

#include <cstdint>

namespace Utf8Input {

void AppendCodepoint(std::string *s, int codepoint) {
  if (codepoint <= 0) {
    return;
  }
  if (codepoint < 0x80) {
    s->push_back(static_cast<char>(codepoint));
  } else if (codepoint < 0x800) {
    s->push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
    s->push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  } else if (codepoint < 0x10000) {
    s->push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
    s->push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
    s->push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
  }
}

void PopLastChar(std::string *s) {
  if (s->empty()) {
    return;
  }
  size_t i = s->size() - 1;
  while (i > 0 && (static_cast<std::uint8_t>((*s)[i]) & 0xC0) == 0x80) {
    --i;
  }
  s->erase(i);
}

}  // namespace Utf8Input
