#ifndef AZULINDO_SCREEN_DIALOGUE_PANEL_H_
#define AZULINDO_SCREEN_DIALOGUE_PANEL_H_

#include <mutex>
#include <string>

#include "raylib.h"

class DialoguePanel {
 public:
  void UpdateLayout(int screen_width, int screen_height);
  Rectangle TextArea() const;
  void Draw(const std::string &text) const;

 private:
  Rectangle bounds_{};
  mutable std::mutex layout_mutex_;
};

#endif  // AZULINDO_SCREEN_DIALOGUE_PANEL_H_
