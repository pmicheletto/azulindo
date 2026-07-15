#ifndef AZULINDO_APP_PROMPT_BUILDER_H_
#define AZULINDO_APP_PROMPT_BUILDER_H_

#include <string>

#include "configs/prompt_config.h"

namespace PromptBuilder {

inline std::string BuildChatml(const std::string &user_prompt) {
  return std::string(PromptConfig::kImStartSystem) + PromptConfig::kPersonality +
         "\n" + std::string(PromptConfig::kImEndMarker) + "\n" +
         PromptConfig::kImStartUser + user_prompt + "\n" +
         std::string(PromptConfig::kImEndMarker) + "\n" +
         PromptConfig::kImStartAssistant;
}

}  // namespace PromptBuilder

#endif  // AZULINDO_APP_PROMPT_BUILDER_H_
