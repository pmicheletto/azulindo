#include "app/status_overlay.h"

#include "configs/app_config.h"
#include "raylib.h"

void StatusOverlay::Draw() const {
  DrawFPS(AppConfig::kStatusFpsX, AppConfig::kStatusFpsY);
}
