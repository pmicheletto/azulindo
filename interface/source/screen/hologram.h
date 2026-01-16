#ifndef HOLOGRAM_H
#define HOLOGRAM_H

#include "raylib.h"

namespace HologramConstants {
struct CameraConfig {
  static constexpr float position_x = 0.0f;
  static constexpr float position_y = 0.5f;
  static constexpr float position_z = 2.0f;
  static constexpr float target_x = 0.0f;
  static constexpr float target_y = 0.0f;
  static constexpr float target_z = 0.0f;
  static constexpr float up_x = 0.0f;
  static constexpr float up_y = 1.0f;
  static constexpr float up_z = 0.0f;
  static constexpr float fovy = 45.0f;
};

struct ModelConfig {
  static constexpr float position_x = 0.6f;
  static constexpr float position_y = -0.36f;
  static constexpr float position_z = 0.4f;
  static constexpr float rotation_axis_x = 0.0f;
  static constexpr float rotation_axis_y = 1.0f;
  static constexpr float rotation_axis_z = 0.0f;
  static constexpr float scale_x = 0.0025f;
  static constexpr float scale_y = 0.0025f;
  static constexpr float scale_z = 0.0025f;
};

struct AnimationConfig {
  static constexpr float rotation_speed_degrees_per_second = 45.0f;
  static constexpr float color_fade_alpha = 0.2f;
};
}  // namespace HologramConstants

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
