#ifndef AZULINDO_SCREEN_H_
#define AZULINDO_SCREEN_H_

#include <unistd.h>

#include <atomic>
#include <algorithm>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "core/state_emotion/emotion_profile.h"
#include "hologram.h"
#include "configs/layout_config.h"
#include "raylib.h"
#include "raymath.h"

class AzulindoScreen {
 public:
  AzulindoScreen(int screen_width, int screen_height);
  ~AzulindoScreen();

  void Update(float delta_time);
  void Draw();
  void SetEmotion(EmotionState emotion);
  void AppendAiText(const std::string& text);
  void ClearAiText();
  void SetPauseWaveWhileAiOutput(bool pause);
  bool IsAiTextPipelineBusy() const;

 private:
  void UpdateLayout();
  void UpdateEmotion(float delta_time);

  void DrawBackgroundLines() const;
  void DrawDialogueBox() const;
  void DrawHud() const;
  void DrawWave() const;
  void DrawTextWrapped(Font font, const char* text, Rectangle rec,
                       float fontSize, float spacing, Color color) const;
  float MeasureWrappedContentHeight(Font font, const char* text, float rec_width,
                                    float fontSize, float spacing) const;
  Rectangle DialogueTextArea() const;
  void TextIngestLoop();

  int screen_width_;
  int screen_height_;
  bool use_full_screen_;
  float timer_ = 0.0f;

  std::string ai_response_ = "AZULINDO: Sistema inicializado...";

  Rectangle dialogue_bounds_;
  EmotionState current_emotion_ = EmotionState::kIdle;
  EmotionState target_emotion_ = EmotionState::kIdle;

  WaveConfig wave_config_;

  std::unique_ptr<Hologram> car_hologram_;

  mutable std::mutex text_mutex_;
  std::mutex layout_mutex_;
  Rectangle dialogue_text_area_{};

  std::thread text_worker_;
  std::atomic<bool> text_worker_running_{true};
  mutable std::mutex ingest_mutex_;
  std::condition_variable ingest_cv_;
  std::queue<std::string> text_ingest_queue_;
  std::atomic<bool> text_ingest_active_{false};
  bool pause_wave_while_ai_output_ = false;
};

#endif  // AZULINDO_SCREEN_H_
