#include <Arduino.h>
#include <digitalWriteFast.h>
#include "ledcube.h"
#include "effects/fireworks.h"

#define pinStatus 3
#define pinSW1 17
#define pinSW2 18

#define EF_FIREWORK     2
#define EF_RAIN         0
#define EF_GROWFRAME    1
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

void randomFillEffect(int count){
  while(count>0){
    bool ledTrack[512]={false};
    bool setval=true;
    for(int iii=0; iii<2; iii++){
      for(int i=0; i<512; i++){
        int ii;
        do {
          ii=random(513);
        }while(ledTrack[ii]==setval);
        ledTrack[ii]=setval;
        cube.writeLED(ii%8, (ii>>3)%8, (ii>>6)%8, setval);
        cube.update();
        cube.waitForFrame();
      }
      setval=false;
    }
    count--;
  }
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

void growFrameEffectCube(byte pos[], int dir[]){
  //draw the frame
  for(int p=8; p>=0; p--){
    for(int i=0; i<6; i++){
      if ((pos[0]<=p)&&(p<=pos[3])) cube.writeLED(p,pos[1+((i>>1)&1)*3],pos[2+(i&1)*3],HIGH);
      if ((pos[1]<=p)&&(p<=pos[4])) cube.writeLED(pos[  ((i>>1)&1)*3],p,pos[2+(i&1)*3],HIGH);
      if ((pos[2]<=p)&&(p<=pos[5])) cube.writeLED(pos[  ((i>>1)&1)*3],pos[1+(i&1)*3],p,HIGH);
    }
  }

  //scale the cube
  for(int i=0; i<6; i++){
    pos[i]+=dir[i];
  }

  //if we're about to collapse or expand past limits:
  if ( pos[0] >= pos[3] ) {

    //flip the directions
    for(int i=0; i<6; i++){
      dir[i]*=-1;
    }

    //back out one
    for(int i=0; i<6; i++){
      pos[i]+=dir[i];
    }
  }

  //the cube will scale as three of these values travel towards the opposite extreme, then move them back
  cube.update();
  cube.waitForFrame(4);
  cube.clearPattern();
}

void growframeEffect(int count){
  while(count>0){

    //the cube is defined by two points
    //initialize cordinates for two points
    byte pos[6]={
      0,0,0,
      7,7,7,
    };
    int dir[6];

    for(byte dirmask=0; dirmask<8; dirmask++){
      //use dirmask to set dir[0] through dir[6]
      for(int i=0; i<3; i++){
        if( ( (dirmask>>i)&1 )==1){
          dir[i]=0;
          dir[i+3]=-1;
        }
        else{
          dir[i]=1;
          dir[i+3]=0;
        }
      }

      for(int keyframe=0; keyframe<13; keyframe++){
        growFrameEffectCube(pos,dir);
      }
    }
    //okay now make the whole thing collapse in and out
    for(int i=0; i<3; i++){
      dir[i]=1;
      dir[i+3]=-1;
    }

    for (int i=0; i<4; i++){
      for(int keyframe=0; keyframe<8; keyframe++){
        growFrameEffectCube(pos,dir);
      }
    }
    count--;
  }
}

void loop() {
  switch(effectIndex){
    case 4:
      rainEffect(400);
      break;
    case 3:
      fireworkEffect(8);
      break;
    case 2:
      growframeEffect(1);
      break;
    case 1:
      planeMoveEffect(2);
      break;
    case 0:
      randomFillEffect(1);
      break;
  }
if (play) effectIndex++;
cube.clearPattern();
cube.waitForFrame();
if (effectIndex>4) effectIndex=0;
}
