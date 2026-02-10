#include "emotion_manager.h"

EmotionManager& EmotionManager::Instance() {
  static EmotionManager instance;
  return instance;
}

void EmotionManager::SetState(EmotionState new_state) {
  std::lock_guard<std::mutex> lock(state_mutex_);
  current_state_ = new_state;
}

EmotionState EmotionManager::GetCurrentState() const {
  std::lock_guard<std::mutex> lock(state_mutex_);
  return current_state_;
}

const EmotionProfile& EmotionManager::GetCurrentProfile() const {
  return GetEmotionProfile(GetCurrentState());
}