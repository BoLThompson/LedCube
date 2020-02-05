#ifndef GROWFRAME_H
#define GROWFRAME_H
#include <Arduino.h>
#include "../ledcube.h"

extern LedCube cube;

class EF_GROWFRAME : public EFFECT {
private:
  int phase=0;
  int tick=0;
  void drawFrame();
  void scaleFrame();
  byte pos[6];
  int dir[6];
public:
  EF_GROWFRAME();
  bool step() override;
};

#endif
