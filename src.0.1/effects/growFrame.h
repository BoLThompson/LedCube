#ifndef GROWFRAME_H
#define GROWFRAME_H
#include <Arduino.h>
#include "../ledcube.h"

class EF_GROWFRAME{
private:
  int tick=0;
  LedCube *cube;
  void drawandscale();
  byte pos[6];
  int dir[6];
public:
  EF_GROWFRAME(LedCube *arg);
  bool step();
};


#endif
