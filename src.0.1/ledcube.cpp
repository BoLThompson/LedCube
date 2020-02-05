#include "ledcube.h"
#include <Arduino.h>
#include <digitalWriteFast.h>

inline void LedCube::waitForLayer(){
  while(!layershown);
  layershown=false;
}

void LedCube::waitForFrame(int count){
  do {
    waitForFrame();
    count--;
  } while (count>0);
}

void LedCube::waitForFrame(){
  do {
    waitForLayer();
  } while (plane!=7);
}

void LedCube::layerInc(){
  //pick the next layer
  plane++;
  if (plane>=8) plane=0;

  //set the layer address
  for(int i=0; i<3; i++) {
    digitalWriteFast(pinL[i],((plane&_BV(i))>0));
  }
}

void LedCube::resetAddr() {
  addrindex=0;
  addr=addrpattern[addrindex];
  for (byte i=0; i<3; i++){
    digitalWriteFast(pinA[i],LOW);
  }
}

void LedCube::incAddr() {

    //store the current addr
    byte oldaddr=addr;

    //increment our addrindex
    addrindex++;

    //perform our manipulation to get the next addr
    addr=addrpattern[addrindex];

    //figure out which bit just flipped
    byte diffpattern=addr^oldaddr;

    //figure out which pin is the one that needs to flip
    int changepin = 0;
    while (diffpattern>>changepin!=1) changepin++;

    //make the change
    digitalWriteFast(pinA[changepin],((addr&diffpattern)>0));
}

LedCube::LedCube(){

  //preset the bus pins and layer pins to low
  for (int ii=0; ii<8; ii++){
    pinMode(pinD[ii],OUTPUT);
    digitalWriteFast(pinD[ii],LOW);
    pinMode(pinL[ii],OUTPUT);
    digitalWriteFast(pinL[ii],LOW);
  }

  //preset the address pins to a zero pattern
  for (int ii=0; ii<3; ii++){
    pinMode(pinA[ii],OUTPUT);
    digitalWriteFast(pinA[ii],OUTPUT);
  }

  //raise the OE pin to disable output
  pinMode(pinOE,OUTPUT);
  analogWrite(pinOE,dimness);
}


void LedCube::interrupt() {
  TCNT0=0;
  //turn off the output
  digitalWriteFast(pinOE,HIGH);

  //select the layer
  layerInc();

  resetAddr();

  //for each row, NOT SEQUENTIAL: USE ADDR INSTEAD OF ROW
  for (byte row=0; row<8; row++) {

    //set all the bus pins to the bits in the pattern byte
    for (int ii=0; ii<8; ii++){
      digitalWriteFast(pinD[ii],patternbuffer[ii][plane]&_BV(addr));
    }
    // for (int ii=0; ii<8; ii++){
    //   digitalWriteFast(pinD[ii],ledbuffer[ii][addr][plane]);
    // }

    //next address
    incAddr();
  }

  //turn on the output
  digitalWriteFast(pinOE,LOW);

  layershown=true;
}

void LedCube::update(){
  cli();
  for(byte i=0; i<8; i++){
    for(byte ii=0; ii<8; ii++){
      patternbuffer[i][ii]=pattern[i][ii];
    }
  }
  sei();
}

void LedCube::writeLED(byte x, byte y, byte z, bool value){
  if (value) {
    pattern[y][z]=pattern[y][z]|_BV(x);
  }
  else {
    pattern[y][z]=pattern[y][z]&~_BV(x);
  }
}

void LedCube::clearPattern(){
  for (int i=0; i<8; i++){
    for (int ii=0; ii<8; ii++){
      pattern[i][ii]=0;
    }
  }
}
