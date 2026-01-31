#ifndef HOLOGRAM_H
#define HOLOGRAM_H

#include "raylib.h"
#include "configs/layout_config.h"

class Hologram {
 public:
  Hologram(const char* modelPath);
  ~Hologram();

  void Update(float dt);
  void Draw(Color color);

 private:
  Model model_;
  Camera3D camera_;
  float rotation_;
};

#endif  // HOLOGRAM_H
