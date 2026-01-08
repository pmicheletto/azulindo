#include <cmath>
#include <vector>

#include "raylib.h"
#include "raymath.h"

struct WaveConfig {
  float frequency = 0.02f;
  float speed = 5.0f;
  Color core_color = SKYBLUE;
  Color glow_color = {80, 160, 255, 200};
};

void DrawBackgroundlines(int width, int height) {
  for (int i = 0; i < height; i += 4) {
    DrawRectangle(0, i, width, 1, {50, 100, 255, 12});
  }
}

void DrawHud(int screen_width) {
  DrawText("SYSTEM: AZULINDO ACTIVE", 20, 20, 15, SKYBLUE);
  DrawText("AI ENGINE: GEMINI 1.5", 20, 40, 12, BLUE);

  // Side panel logs
  DrawRectangle(screen_width - 200, 20, 180, 80, {30, 60, 150, 40});
  DrawText("AZULINDO LOGS:", screen_width - 190, 30, 10, SKYBLUE);
  DrawText("- Wave Engine: STABLE", screen_width - 190, 50, 10, LIGHTGRAY);
  DrawText("- Visuals: ENHANCED", screen_width - 190, 65, 10, LIGHTGRAY);
}

void DrawAzulindoWave(int screen_width, int center_y, float timer, float intensity, WaveConfig config) {
  for (int x = 0; x < screen_width; x += 2) {
    float y1 = sinf(x * config.frequency + timer * config.speed) * intensity;
    float y2 = sinf(x * config.frequency + timer * config.speed + PI/2) * intensity;

    float harmonic = sinf(x * 0.05f - timer * 2.0f) * (intensity * 0.2f);
    y1 += harmonic;
    y2 -= harmonic;

    int pos_y1 = center_y + static_cast<int>(y1);
    int pos_y2 = center_y + static_cast<int>(y2);

    if ( x > screen_width/2 ) {
      DrawLine(x, pos_y2, center_y, pos_y1, Fade(config.core_color, 0.1f));
    } else {
      DrawLine(center_y, pos_y2, x, pos_y1, Fade(config.core_color, 0.1f));
    }
    //DrawLine(center_y, pos_y1, x, pos_y2, Fade(config.core_color, 0.1f)); //change this to  edit the lines behinde the wave
    //DrawLine(center_y, center_y, x, pos_y1, Fade(config.core_color, 0.1f)); //change this to edit the lines behinde the wave

    for (int layer = 1; layer <= 3; ++layer) {
      float radius = static_cast<float>(4 - layer);
      Color layer_color = Fade(config.glow_color, 0.1f / layer);
      
      //DrawCircle(x, pos_y1, radius, layer_color);
      DrawCircle(x, pos_y2, radius, layer_color);
    }
  }
}

int main() {
  const int screen_width = 480;
  const int screen_height = 480;

  InitWindow(screen_width, screen_height, "Azulindo - Modular System");
  SetTargetFPS(60);

  float timer = 0.0f;
  float intensity = 25.0f;
  WaveConfig azulindo_style;

  while (!WindowShouldClose()) {
    timer += GetFrameTime();

    if (IsKeyDown(KEY_SPACE)) {
      intensity = Lerp(intensity, 100.0f, 0.05f);
    } else {
      intensity = Lerp(intensity, 25.0f, 0.05f);
    }

    BeginDrawing();
    ClearBackground({2, 8, 20, 255});

    DrawBackgroundlines(screen_width, screen_height);
    
    DrawAzulindoWave(screen_width, screen_height / 2, timer, intensity, azulindo_style);
    
    DrawHud(screen_width);

    DrawText("9854", 50, (screen_height / 2) - 80, 10, DARKBLUE);
    DrawText("65147", screen_width - 80, (screen_height / 2) + 80, 10, DARKBLUE);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}