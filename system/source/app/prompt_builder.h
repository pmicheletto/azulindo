#ifndef AZULINDO_APP_PROMPT_BUILDER_H_
#define AZULINDO_APP_PROMPT_BUILDER_H_

#include <string>
#include <vector>

#include "configs/prompt_config.h"
#include "llama.h"

namespace PromptBuilder {

struct Turn {
  std::string user;
  std::string assistant;
};

namespace detail {

inline std::string ApplyChatTemplate(const llama_chat_message *messages,
                                     std::size_t n_msg, bool add_assistant) {
  // "chatml" matches Qwen markers. Full Qwen3 Jinja is not run here; we append
  // kDisableThinkingPrefix after the assistant opener ourselves.
  std::vector<char> buf(n_msg * 512 + 128);
  int32_t n = llama_chat_apply_template(
      "chatml", messages, n_msg, add_assistant, buf.data(),
      static_cast<int32_t>(buf.size()));
  if (n < 0) {
    return {};
  }
  if (n > static_cast<int32_t>(buf.size())) {
    buf.resize(static_cast<std::size_t>(n));
    n = llama_chat_apply_template("chatml", messages, n_msg, add_assistant,
                                  buf.data(), static_cast<int32_t>(buf.size()));
    if (n < 0) {
      return {};
    }
  }
  return std::string(buf.data(), static_cast<std::size_t>(n));
}

}  // namespace detail

// Cached KV prefix: system role only (reused across turns in Brain).
inline std::string BuildSystemPrefix(
    PromptConfig::Language language = PromptConfig::kDefaultLanguage) {
  const llama_chat_message msgs[] = {
      {"system", PromptConfig::PersonalityFor(language)},
  };
  return detail::ApplyChatTemplate(msgs, 1, /*add_assistant=*/false);
}

inline std::string BuildChatml(
    const std::string &user_prompt, const std::vector<Turn> &history = {},
    PromptConfig::Language language = PromptConfig::kDefaultLanguage) {
  std::vector<llama_chat_message> msgs;
  msgs.reserve(2 + history.size() * 2);
  msgs.push_back({"system", PromptConfig::PersonalityFor(language)});
  for (const Turn &turn : history) {
    msgs.push_back({"user", turn.user.c_str()});
    msgs.push_back({"assistant", turn.assistant.c_str()});
  }
  msgs.push_back({"user", user_prompt.c_str()});
  // ChatML ends at <|im_start|>assistant\n. Append Qwen3's official
  // enable_thinking=false prefill so generation starts on the visible reply.
  return detail::ApplyChatTemplate(msgs.data(), msgs.size(),
                                   /*add_assistant=*/true) +
         PromptConfig::kDisableThinkingPrefix;
}

}  // namespace PromptBuilder

#endif  // AZULINDO_APP_PROMPT_BUILDER_H_
