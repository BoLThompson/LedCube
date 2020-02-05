#ifndef LEDCUBE_H
#define LEDCUBE_H
#include <Arduino.h>

// OE  B4  12
#define pinOE 11//12

// A0  B3  11
#define pinA0 10//11

// A1  B2  10
#define pinA1 12//10

// A2  B5  13
#define pinA2 13//13

// D0  B1  13
#define pinD0 9//9

// D1  B0  8
#define pinD1 8//8

// D2  D7  7
#define pinD2 7//7

// D3  D6  6
#define pinD3 6//6

// D4  D5  5
#define pinD4 5//5

// D5  D4  4
#define pinD5 4//4

// D6  D3  3
#define pinD6 19//3

// D7  D2  2
#define pinD7 2//2

// L0  C4  18
#define pinL0 16//18

// L1  C5  19
#define pinL1 15//19

// L2      a6?
#define pinL2 14//-1

class LedCube{
private:
  const int pinD[8] = {
    pinD0,
    pinD1,
    pinD2,
    pinD3,
    pinD4,
    pinD5,
    pinD6,
    pinD7,
  };
  const byte addrpattern[9]={
    0B000,
    0B001,
    0B011,
    0B010,
    0B110,
    0B100,
    0B101,
    0B111,
    0B011,
  };
  const int pinL[3] = {
    pinL0,
    pinL1,
    pinL2,
  };
  volatile byte addr=0;
  const int pinA[3] = {pinA0,pinA1,pinA2};
  volatile byte addrindex=0;
  volatile byte patternbuffer[8][8];
  volatile bool layershown={true};
  volatile byte plane=7;
  byte dimness=0;
  inline void waitForLayer();
  void layerInc();
  void resetAddr();
  void incAddr();

public:
  byte pattern[8][8];
  LedCube();
  void waitForFrame();
  void waitForFrame(int count);
  void update();
  void writeLED(byte x, byte y, byte z, bool value);
  void clearPattern();
  void interrupt();
};

#endif
