#include "core/brain/brain.h"
#include "core/state_emotion/emotion.h"
#include "raylib.h"
#include "screen/azulindo_screen.h"

int main() {
    int current_width = 800;
    int current_height = 480;
    InitWindow(current_width, current_height, "Azulindo - Ford Ka 2002");
    SetTargetFPS(60);

    // Instâncias
    AzulindoScreen screen(current_width, current_height);
    Brain azulindo_brain;

    while (!WindowShouldClose()) {
        
        std::string token = azulindo_brain.PopToken(); // Sem o WHILE aqui!

        if (!token.empty()) {
            screen.AppendAiText(token);
            screen.SetEmotion(EmotionState::kIdle);
        }

        if (IsKeyPressed(KEY_ONE)) {
            screen.SetEmotion(EmotionState::kIdle);
        } else if (IsKeyPressed(KEY_TWO)) {
            screen.SetEmotion(EmotionState::kAngry);
        } else if (IsKeyPressed(KEY_THREE)) {
            screen.SetEmotion(EmotionState::kThinking);
        }

        const bool ask_key = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_SLASH) ||
                             IsKeyPressed(KEY_KP_DIVIDE);
        
        if (ask_key) {
            std::string prompt = "<|system|>\nVoce é o Azulindo, a IA de um Ford Ka.</s>\n<|user|>\nOi!</s>\n<|assistant|>\n";
            if (azulindo_brain.RequestInference(prompt)) {
                screen.ClearAiText();
                screen.SetEmotion(EmotionState::kThinking);
            }
        }

        const bool ai_output_active =
            azulindo_brain.IsStreamingTokens() || screen.IsAiTextPipelineBusy();
        screen.SetPauseWaveWhileAiOutput(ai_output_active);

        screen.Update(GetFrameTime());

        BeginDrawing();
            screen.Draw();

            DrawFPS(10, 10);

            if (!azulindo_brain.LlamaReady()) {
                DrawText("LLM offline: verifique o arquivo .gguf", 10, 32, 18, ORANGE);
            } else {
                DrawText("Espaco ou / : perguntar ao Azulindo", 10, 32, 18, Fade(LIGHTGRAY, 0.6f));
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}