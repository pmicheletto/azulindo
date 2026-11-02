#ifndef AZULINDO_APP_TOKEN_STREAM_CONTROLLER_H_
#define AZULINDO_APP_TOKEN_STREAM_CONTROLLER_H_

#include <optional>
#include <string>

#include "app/think_block_filter.h"

class Brain;
class AzulindoScreen;

class TokenStreamController {
 public:
  void ResetCooldown();
  void Update(Brain &brain, AzulindoScreen &screen, float delta_time);

  // Returns the finished assistant reply once per completed generation.
  std::optional<std::string> TakeCompletedReply();

 private:
  float display_cooldown_ = 0.0f;
  bool speech_idle_applied_ = false;
  bool was_streaming_ = false;
  std::string accumulating_;
  std::optional<std::string> completed_reply_;
  ThinkBlockFilter think_filter_;
};

#endif  // AZULINDO_APP_TOKEN_STREAM_CONTROLLER_H_
