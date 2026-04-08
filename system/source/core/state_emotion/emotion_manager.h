#ifndef AZULINDO_CORE_STATE_EMOTION_MANAGER_H_
#define AZULINDO_CORE_STATE_EMOTION_MANAGER_H_

#include <mutex>

#include "emotion_profile.h"

class EmotionManager {
 public:
  static EmotionManager &Instance();

  void SetState(EmotionState new_state);
  EmotionState GetCurrentState() const;
  const EmotionProfile &GetCurrentProfile() const;

 private:
  EmotionManager() = default;

  EmotionState current_state_ = EmotionState::kIdle;
  mutable std::mutex state_mutex_;
};

#endif
