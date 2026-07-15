#ifndef AZULINDO_APP_TOKEN_STREAM_CONTROLLER_H_
#define AZULINDO_APP_TOKEN_STREAM_CONTROLLER_H_

class Brain;
class AzulindoScreen;

class TokenStreamController {
 public:
  void ResetCooldown();
  void Update(Brain &brain, AzulindoScreen &screen, float delta_time);

 private:
  float display_cooldown_ = 0.0f;
  bool speech_idle_applied_ = false;
};

#endif  // AZULINDO_APP_TOKEN_STREAM_CONTROLLER_H_
