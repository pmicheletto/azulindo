#include "app/status_overlay.h"

#include "configs/app_config.h"
#include "core/brain/brain.h"
#include "raylib.h"

void StatusOverlay::Draw(const Brain &brain) const {
  DrawFPS(10, 50);
  if (!brain.LlamaReady()) {
    DrawText(AppConfig::kHintLlmOffline, 10, 32, 18, ORANGE);
  }
}
