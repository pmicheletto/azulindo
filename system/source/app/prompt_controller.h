#ifndef AZULINDO_APP_PROMPT_CONTROLLER_H_
#define AZULINDO_APP_PROMPT_CONTROLLER_H_

#include <string>
#include <vector>

#include "app/prompt_builder.h"
#include "configs/prompt_config.h"

class Brain;
class AzulindoScreen;

class PromptController {
 public:
  // Returns true when a new inference request was accepted.
  bool Update(Brain &brain, AzulindoScreen &screen,
              PromptConfig::Language language);
  void Draw(const Brain &brain) const;

  // Call when the assistant finishes streaming a reply.
  void OnAssistantReply(std::string reply);

  // Drop chat history after a language switch so tone/language don't mix.
  void OnLanguageChanged();

 private:
  std::string input_;
  bool focused_ = false;
  std::string pending_user_;
  std::vector<PromptBuilder::Turn> history_;
  PromptConfig::Language language_ = PromptConfig::kDefaultLanguage;
};

#endif  // AZULINDO_APP_PROMPT_CONTROLLER_H_
