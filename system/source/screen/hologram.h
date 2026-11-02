#ifndef AZULINDO_SCREEN_HOLOGRAM_H_
#define AZULINDO_SCREEN_HOLOGRAM_H_

#include "configs/layout_config.h"
#include "raylib.h"

class Hologram {
 public:
  explicit Hologram(const char *model_path);
  ~Hologram();

  Hologram(const Hologram &) = delete;
  Hologram &operator=(const Hologram &) = delete;

  void Update(float dt);
  void Draw(Color color);

 private:
  Model model_{};
  Camera3D camera_{};
  float rotation_ = 0.0f;
};

#endif  // AZULINDO_SCREEN_HOLOGRAM_H_
