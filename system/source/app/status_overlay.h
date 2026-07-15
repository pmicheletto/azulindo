#ifndef AZULINDO_APP_STATUS_OVERLAY_H_
#define AZULINDO_APP_STATUS_OVERLAY_H_

class Brain;

class StatusOverlay {
 public:
  void Draw(const Brain &brain) const;
};

#endif  // AZULINDO_APP_STATUS_OVERLAY_H_
