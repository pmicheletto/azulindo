#include "app/utf8_input.h"

#include <cstdint>

#include "raylib.h"

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
  } else if (codepoint <= 0x10FFFF) {
    s->push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
    s->push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
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

std::string AsciiFallback(int codepoint) {
  switch (codepoint) {
    case 0x2010:
    case 0x2011:
    case 0x2012:
    case 0x2013:  // en dash
    case 0x2014:  // em dash
    case 0x2015:
    case 0x2212:
      return "-";
    case 0x2018:
    case 0x2019:
    case 0x201A:
    case 0x2032:
      return "'";
    case 0x201C:
    case 0x201D:
    case 0x201E:
    case 0x2033:
      return "\"";
    case 0x2026:
      return "...";
    case 0x00A0:
      return " ";

    // Latin accents -> plain ASCII (HUD font has no glyphs; do not drop).
    case 0x00C0:
    case 0x00C1:
    case 0x00C2:
    case 0x00C3:
    case 0x00C4:
    case 0x00C5:
      return "A";
    case 0x00E0:
    case 0x00E1:
    case 0x00E2:
    case 0x00E3:
    case 0x00E4:
    case 0x00E5:
      return "a";
    case 0x00C7:
      return "C";
    case 0x00E7:
      return "c";
    case 0x00C8:
    case 0x00C9:
    case 0x00CA:
    case 0x00CB:
      return "E";
    case 0x00E8:
    case 0x00E9:
    case 0x00EA:
    case 0x00EB:
      return "e";
    case 0x00CC:
    case 0x00CD:
    case 0x00CE:
    case 0x00CF:
      return "I";
    case 0x00EC:
    case 0x00ED:
    case 0x00EE:
    case 0x00EF:
      return "i";
    case 0x00D1:
      return "N";
    case 0x00F1:
      return "n";
    case 0x00D2:
    case 0x00D3:
    case 0x00D4:
    case 0x00D5:
    case 0x00D6:
      return "O";
    case 0x00F2:
    case 0x00F3:
    case 0x00F4:
    case 0x00F5:
    case 0x00F6:
      return "o";
    case 0x00D9:
    case 0x00DA:
    case 0x00DB:
    case 0x00DC:
      return "U";
    case 0x00F9:
    case 0x00FA:
    case 0x00FB:
    case 0x00FC:
      return "u";
    case 0x00DD:
      return "Y";
    case 0x00FD:
    case 0x00FF:
      return "y";

    default:
      return {};
  }
}

namespace {

bool IsEmojiOrSymbol(int codepoint) {
  // Drop anything that tends to render as '?' on the HUD font.
  if (codepoint == 0xFE0F || codepoint == 0x200D || codepoint == 0x20E3) {
    return true;
  }
  if (codepoint >= 0x2600 && codepoint <= 0x27BF) {
    return true;  // misc symbols / dingbats
  }
  if (codepoint >= 0x1F000 && codepoint <= 0x1FAFF) {
    return true;  // emoji blocks
  }
  return false;
}

}  // namespace

std::string SanitizeForHudFont(const std::string &utf8) {
  std::string out;
  out.reserve(utf8.size());
  const int length = static_cast<int>(utf8.size());
  for (int i = 0; i < length;) {
    int size = 0;
    const int codepoint = GetCodepoint(&utf8[static_cast<size_t>(i)], &size);
    if (size <= 0) {
      ++i;
      continue;
    }
    if (IsEmojiOrSymbol(codepoint)) {
      i += size;
      continue;
    }
    // Prefer ASCII; accented letters are folded via AsciiFallback (not dropped).
    if (codepoint == '\n' || (codepoint >= 0x20 && codepoint <= 0x7E)) {
      out.append(utf8, static_cast<size_t>(i), static_cast<size_t>(size));
    } else {
      out += AsciiFallback(codepoint);
    }
    i += size;
  }
  return out;
}

}  // namespace Utf8Input
