#include "hologram.h"

Hologram::Hologram(const char* modelPath) : rotation_(0.0f) {
  using HConfig = LayoutConfig::HologramConfig::CameraConfig;

  model_ = LoadModel(modelPath);
  camera_.position = (Vector3){HConfig::position_x, HConfig::position_y,
                               HConfig::position_z};
  camera_.target = (Vector3){HConfig::target_x, HConfig::target_y,
                             HConfig::target_z};
  camera_.up = (Vector3){HConfig::up_x, HConfig::up_y, HConfig::up_z};
  camera_.fovy = HConfig::fovy;
  camera_.projection = CAMERA_PERSPECTIVE;
}

Hologram::~Hologram() { UnloadModel(model_); }

void Hologram::Update(float dt) {
  using AConfig = LayoutConfig::HologramConfig::AnimationConfig;
  rotation_ += dt * AConfig::rotation_speed_degrees_per_second;
}

void Hologram::Draw(Color color) {
  using HModel = LayoutConfig::HologramConfig::ModelConfig;
  using AConfig = LayoutConfig::HologramConfig::AnimationConfig;

  BeginMode3D(camera_);
  // DrawGrid(10, 1.0f); // if necessary for while debugging
  Color hologramaColor = Fade(color, AConfig::color_fade_alpha);

  const Vector3 model_position = {HModel::position_x, HModel::position_y,
                                  HModel::position_z};
  const Vector3 model_rotation_axis = {HModel::rotation_axis_x,
                                       HModel::rotation_axis_y,
                                       HModel::rotation_axis_z};
  const Vector3 model_scale = {HModel::scale_x, HModel::scale_y,
                               HModel::scale_z};

  DrawModelWiresEx(model_, model_position, model_rotation_axis, rotation_,
                   model_scale, hologramaColor);
  // DrawModelEx(model_, model_position, model_rotation_axis,  //TODO:  decide
  // which one is better rotation_, model_scale, hologramaColor);
  EndMode3D();
}