#ifndef PLANEMOVE_H
#define PLANEMOVE_H
#include <Arduino.h>
#include "../ledcube.h"

extern LedCube cube;
extern bool play;

class EF_PLANEMOVE : public EFFECT {
private:
  int tick=0;
  int phase=0;
  int dir=1;
  int pos[3];
public:
  EF_PLANEMOVE();
  bool step() override;
};
#endif