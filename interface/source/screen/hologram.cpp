#include "hologram.h"

Hologram::Hologram(const char* modelPath) : rotation_(0.0f) {
  using namespace HologramConstants;

  model_ = LoadModel(modelPath);
  camera_.position =
      (Vector3){CameraConfig::position_x, CameraConfig::position_y,
                CameraConfig::position_z};
  camera_.target = (Vector3){CameraConfig::target_x, CameraConfig::target_y,
                             CameraConfig::target_z};
  camera_.up =
      (Vector3){CameraConfig::up_x, CameraConfig::up_y, CameraConfig::up_z};
  camera_.fovy = CameraConfig::fovy;
  camera_.projection = CAMERA_PERSPECTIVE;
}

Hologram::~Hologram() { UnloadModel(model_); }

void Hologram::Update(float dt) {
  using namespace HologramConstants;
  rotation_ += dt * AnimationConfig::rotation_speed_degrees_per_second;
}

void Hologram::Draw(Color color) {
  using namespace HologramConstants;

  BeginMode3D(camera_);
  // DrawGrid(10, 1.0f); // if necessary for while debugging
  Color hologramaColor = Fade(color, AnimationConfig::color_fade_alpha);

  const Vector3 model_position = {ModelConfig::position_x,
                                  ModelConfig::position_y,
                                  ModelConfig::position_z};
  const Vector3 model_rotation_axis = {ModelConfig::rotation_axis_x,
                                       ModelConfig::rotation_axis_y,
                                       ModelConfig::rotation_axis_z};
  const Vector3 model_scale = {ModelConfig::scale_x, ModelConfig::scale_y,
                               ModelConfig::scale_z};

  DrawModelWiresEx(model_, model_position, model_rotation_axis, rotation_,
                   model_scale, hologramaColor);
  // DrawModelEx(model_, model_position, model_rotation_axis,  //TODO:  decide
  // which one is better rotation_, model_scale, hologramaColor);
  EndMode3D();
}