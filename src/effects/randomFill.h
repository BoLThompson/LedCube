#ifndef RANDOMFILL_H
#define RANDOMFILL_H
#include <Arduino.h>
#include "../ledcube.h"

extern LedCube cube;

class EF_RANDOMFILL : public EFFECT {
private:
  bool ledTrack[512];
  bool setval=true;
  int phase=0;
  int tick=0;
public:
  EF_RANDOMFILL();
  bool step() override;
};

#endif
