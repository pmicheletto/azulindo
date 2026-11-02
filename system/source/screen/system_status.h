#ifndef AZULINDO_SCREEN_SYSTEM_STATUS_H_
#define AZULINDO_SCREEN_SYSTEM_STATUS_H_

// Presentation-level system readiness. App maps Brain state into this;
// screen/HUD never depends on Brain/llama.
enum class SystemStatus {
  kBooting,
  kOnline,
  kOffline,
};

#endif  // AZULINDO_SCREEN_SYSTEM_STATUS_H_
