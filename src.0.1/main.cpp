#include <Arduino.h>
#include <digitalWriteFast.h>
#include "ledcube.h"
#include "effects/fireworks.h"
#include "effects/randomfill.h"
#include "effects/growframe.h"

#define pinStatus 3
#define pinSW1 17
#define pinSW2 18

const int effectCount=2;

int effectIndex=0;
byte dimness=0;
bool play=true;
bool skipflag=false;
bool sw1=false;
bool sw2=false;

LedCube cube;

const uint16_t T0_COMP = 81250;

void setup() {
  randomSeed(analogRead(A7)*analogRead(A6)*analogRead(A2));

  TCCR0A=0;
  TCCR0B=(1<<CS12);
  TCCR0B&=~(1<<CS11);
  TCCR0B&=~(1<<CS10);
  TCNT0=0;
  OCR0A=T0_COMP;
  TIMSK0=(1<<OCIE0A);
  sei();

  //play mode indicated by no light
  pinMode(pinStatus,INPUT);
}

ISR(TIMER0_COMPA_vect){
  cube.interrupt();

  bool cursw1=digitalReadFast(pinSW1);
  bool cursw2=digitalReadFast(pinSW2);
  if (cursw1&&!sw1){
    play=true;
    pinMode(pinStatus,INPUT);
  }
  if (cursw2&&!sw2){
    play=false;
    pinMode(pinStatus,OUTPUT);
    digitalWriteFast(pinStatus,LOW);
  }
  sw1=cursw1;
  sw2=cursw2;
}

void planeMoveEffect(int count){
  while(count>0){
    //do each of the axes
    for(int axis=0; axis<3; axis++){
      //start forwards
      int dir=1;
      int pos[3];
      for(pos[axis%3]=0; pos[axis%3]+dir>=0; pos[axis%3]+=dir){
        //draw the plane
        for(pos[(axis+1)%3]=0; pos[(axis+1)%3]<8; pos[(axis+1)%3]++){
          for(pos[(axis+2)%3]=0; pos[(axis+2)%3]<8; pos[(axis+2)%3]++){
            cube.writeLED(pos[0],pos[1],pos[2],HIGH);
          }
        }
        //flip directions
        cube.update();
        cube.waitForFrame(4);
        cube.clearPattern();
        if (pos[axis%3]==7) dir=-1;
      }
    }
    count--;
  }
}

void rainEffect(int count){
  int exitcondition=0;
  while(exitcondition<8){
    //move the whole cube buffer down a voxel
    for (int y=0; y<8; y++){
      for(int z=0; z<7; z++){
        cube.pattern[y][z]=cube.pattern[y][z+1];
      }
      cube.pattern[y][7]=0;
    }
    //if there's a count, light a random voxel
    if (count>0) {
      cube.writeLED(random(0,8),random(0,8),7,HIGH);
      cube.writeLED(random(0,8),random(0,8),7,HIGH);
      if (play) count--;
    }
    //if there's no count, add one to our exit condition
    else exitcondition++;
    cube.update();
    cube.waitForFrame(3);
  }
}

void fireworkEffect(int count){
  #define sparkcount 20
  while(count>0) {
    //create a firework
    //pick a starting coordinate on the bottom floor
    byte pos[3]={(byte)random(2,6),(byte)random(2,6),0};


    //let the firework rise until its fuse runs out
    for(byte fuse={(byte)random(4,8)};fuse>1;fuse--){
      //highlight the rocket
      cube.writeLED(pos[0],pos[1],pos[2],HIGH);

      //paint image and wait
      cube.update();
      cube.waitForFrame(4);

      //erase rocket position
      cube.writeLED(pos[0],pos[1],pos[2],LOW);

      //escalate rocket and burn the fuse
      pos[2]+=1;
    }

    cube.writeLED(pos[0],pos[1],pos[2],HIGH);
    cube.update();
    cube.waitForFrame(10);
    cube.writeLED(pos[0],pos[1],pos[2],LOW);

    //release sparkybois
    EF_FW_SPARK **sparks = new EF_FW_SPARK*[sparkcount];
    for (byte i=0; i<sparkcount; i++){
      sparks[i]=new EF_FW_SPARK;
      sparks[i]->setPosition(pos[0],pos[1],pos[2]);
      sparks[i]->setDirection(0.628*(float)random(0,11),0.628*(float)random(0,11));
      sparks[i]->setSpeed(random(8,16));
      sparks[i]->setGravity(.1);
    }

    for(int i=0; i<36; i++){
      //erase previous positions
      for (int ii=0; ii<sparkcount; ii++){
        if (sparks[ii]->onField()) cube.writeLED((sparks[ii]->getX()),(sparks[ii]->getY()),(sparks[ii]->getZ()),LOW);
      }

      //calculate new positions
      for (int ii=0; ii<sparkcount; ii++){
        sparks[ii]->step();
      }

      //draw new positions
      for (int ii=0; ii<sparkcount; ii++){
        if (sparks[ii]->onField()) cube.writeLED((sparks[ii]->getX()),(sparks[ii]->getY()),(sparks[ii]->getZ()),HIGH);
      }

      cube.update();
      cube.waitForFrame(1);
    }

    //clean up
    for (int i=0; i<sparkcount; i++)
      delete sparks[i];
    delete [] sparks;

    cube.clearPattern();
    cube.update();
    cube.waitForFrame(16);
    count--;
  }
}

void loop() {

  //create an effect object
  EF_GROWFRAME effect(&cube);

  bool proceed;

  do{
    proceed=effect.step();
    //push this cube update and wait for it to display
    cube.update();
    cube.waitForFrame();
  }
  while ((proceed==false)/*&&(skipflag==false)*/);

//   switch(effectIndex){
//     case 4:
//       rainEffect(400);
//       break;
//     case 3:
//       fireworkEffect(8);
//       break;
//     case 2:
//       growframeEffect(1);
//       break;
//     case 1:
//       planeMoveEffect(2);
//       break;
//     case 0:
//       randomFillEffect(1);
//       break;
//   }
// if (play) effectIndex++;
// cube.clearPattern();
// cube.waitForFrame();
// if (effectIndex>4) effectIndex=0;
}
