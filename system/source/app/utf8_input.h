#ifndef AZULINDO_APP_UTF8_INPUT_H_
#define AZULINDO_APP_UTF8_INPUT_H_

#include <string>

namespace Utf8Input {

void AppendCodepoint(std::string *s, int codepoint);
void PopLastChar(std::string *s);

}  // namespace Utf8Input

#endif  // AZULINDO_APP_UTF8_INPUT_H_
