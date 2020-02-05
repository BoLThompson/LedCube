#ifndef RAIN_H
#define RAIN_H
#include <Arduino.h>
#include "../ledcube.h"

extern LedCube cube;
extern bool play;

class EF_RAIN : public EFFECT {
private:
  int tick=0;
  int phase=0;
  public:
  // EF_RAIN();
  bool step() override;
};

#endif