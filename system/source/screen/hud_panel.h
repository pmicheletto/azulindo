#ifndef AZULINDO_SCREEN_HUD_PANEL_H_
#define AZULINDO_SCREEN_HUD_PANEL_H_

#include <string>

#include "screen/system_status.h"

class HudPanel {
 public:
  void Draw(int screen_width, int screen_height, SystemStatus status,
            const std::string &llm_model) const;

 private:
  void DrawSystemBanner(float origin_x, float origin_y,
                        SystemStatus status) const;
  void DrawLogsPanel(float origin_y, int screen_width, SystemStatus status,
                     const std::string &llm_model) const;

  mutable float cached_rss_mb_ = 0.0f;
  mutable double last_rss_sample_time_ = -1.0;
};

#endif  // AZULINDO_SCREEN_HUD_PANEL_H_
