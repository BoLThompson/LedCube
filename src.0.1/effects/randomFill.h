#ifndef RANDOMFILL_H
#define RANDOMFILL_H
#include <Arduino.h>
#include "../ledcube.h"

class EF_RANDOMFILL{
private:
  bool ledTrack[512];
  bool setval=true;
  int phase=0;
  int tick=0;
  LedCube *cube;
public:
  EF_RANDOMFILL(LedCube *arg);
  bool step();
};

#endif
