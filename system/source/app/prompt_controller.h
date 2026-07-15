#ifndef AZULINDO_APP_PROMPT_CONTROLLER_H_
#define AZULINDO_APP_PROMPT_CONTROLLER_H_

#include <string>

class Brain;
class AzulindoScreen;

class PromptController {
 public:
  // Returns true when a new inference request was accepted.
  bool Update(Brain &brain, AzulindoScreen &screen);
  void Draw(const Brain &brain) const;

 private:
  std::string input_;
  bool focused_ = false;
};

#endif  // AZULINDO_APP_PROMPT_CONTROLLER_H_
