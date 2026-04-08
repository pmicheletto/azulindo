#ifndef HOLOGRAM_H_
#define HOLOGRAM_H_

#include "configs/layout_config.h"
#include "raylib.h"

class Hologram {
 public:
  explicit Hologram(const char *model_path);
  ~Hologram();

  void Update(float dt);
  void Draw(Color color);

 private:
  Model model_;
  Camera3D camera_;
  float rotation_;
};

#endif  // HOLOGRAM_H_
