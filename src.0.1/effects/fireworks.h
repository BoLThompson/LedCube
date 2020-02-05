#ifndef FIREWORKS_H
#define FIREWORKS_H
#include <Arduino.h>

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
#endif
