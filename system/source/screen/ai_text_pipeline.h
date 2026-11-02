#ifndef AZULINDO_SCREEN_AI_TEXT_PIPELINE_H_
#define AZULINDO_SCREEN_AI_TEXT_PIPELINE_H_

#include <string>

class DialoguePanel;

// Main-thread only: appends AI tokens and clips when the dialogue box fills.
class AiTextPipeline {
 public:
  explicit AiTextPipeline(DialoguePanel &dialogue);

  void Append(const std::string &text);
  void Clear();
  const std::string &text() const { return ai_response_; }

 private:
  DialoguePanel &dialogue_;
  std::string ai_response_;
};

#endif  // AZULINDO_SCREEN_AI_TEXT_PIPELINE_H_
