#ifndef FIREWORKS_H
#define FIREWORKS_H
#include <Arduino.h>
#include "../ledcube.h"

extern LedCube cube;

class EF_FW_SPARK {
  private:
    int speed;
    long x,y,z;
    float gravity=0;
    float gravmomentum=0;
    int pitch,yaw;
  public:
    EF_FW_SPARK();
    EF_FW_SPARK(byte xarg, byte yarg, byte zarg);
    void setSpeed(byte arg);
    void setPosition(byte xarg, byte yarg, byte zarg);
    void setDirection(int parg, int yarg);
    void setGravity(float arg);
    void step();
    void draw();
    byte getX();
    byte getY();
    byte getZ();
    bool onField();
};

class EF_FIREWORK : public EFFECT {
  #define sparkcount 20
private:
  int phase=0;
  int tick=0;
  int shot=4;
  byte pos[3];
  byte fuse;
  EF_FW_SPARK **sparks = new EF_FW_SPARK*[sparkcount];
  void killsparks();
  void setrocket();
public:
  EF_FIREWORK();
  ~EF_FIREWORK();
  bool step() override;
};
#endif
