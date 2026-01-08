#include <cmath>
#include <vector>

#include "raylib.h"
#include "raymath.h"

int main() {
    const int screen_width = 480;
    const int screen_height = 480;

    InitWindow(screen_width, screen_height, "Azulindo");
    SetTargetFPS(60);

    float timer = 0.0f;
    float intensity = 20.0f;

    while (!WindowShouldClose()) {
        timer += GetFrameTime();

        // Controls wave intensity using keyboard input.
        if (IsKeyDown(KEY_SPACE)) {
            intensity = Lerp(intensity, 120.0f, 0.1f);
        } else {
            intensity = Lerp(intensity, 10.0f, 0.05f);
        }

        BeginDrawing();

        // Background color (dark blue tone).
        ClearBackground({5, 10, 15, 255});

        // Horizontal scanlines to simulate a CRT-like effect.
        for (int i = 0; i < screen_height; i += 2) {
            DrawRectangle(0, i, screen_width, 1, {50, 100, 255, 15});
        }

        // Wave rendering using multiple layers to create a glow effect.
        const int center_y = screen_height / 2;

        for (int layer = 1; layer <= 2; ++layer) {
            const Color color = {
                80,
                static_cast<unsigned char>(120 + 40 * layer),
                255,
                static_cast<unsigned char>(200 / layer),
            };

            for (int x = 0; x < screen_width; x += 2) {
                float y = sin(x * 0.02f + timer * 8.0f) * intensity;
                y += sin(x * 0.05f - timer * 12.0f) * (intensity / 3.0f);

                DrawCircle(x, center_y + y, 4 - layer, color);
            }
        }

        // Main HUD text.
        DrawText("SYSTEM: ONLINE", 20, 20, 15, SKYBLUE);
        DrawText("AI ENGINE: LLAMA-3", 20, 40, 12, BLUE);
        // Side panel for system logs.
        DrawRectangle(screen_width - 250, 20, 230, 100, {30, 60, 150, 40});
        DrawText("LAST LOGS:", screen_width - 240, 30, 10, SKYBLUE);
        DrawText("- Motor Temp: OK", screen_width - 240, 50, 10, LIGHTGRAY);
        DrawText("- Bluetooth Connected", screen_width - 240, 65, 10,
                 LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
