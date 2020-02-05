#include "growFrame.h"
#include <Arduino.h>
#include "../digitalWriteFast.h"
#include "../ledcube.h"

EF_GROWFRAME::EF_GROWFRAME(){
  for (int i=0; i<3; i++){
    pos[i]=0;
    pos[i+3]=7;
  }
}

void EF_GROWFRAME::drawFrame(){
  //draw the frame
  for(int p=8; p>=0; p--){
    for(int i=0; i<6; i++){
      if ((pos[0]<=p)&&(p<=pos[3])) cube.writeLED(p,pos[1+((i>>1)&1)*3],pos[2+(i&1)*3],HIGH);
      if ((pos[1]<=p)&&(p<=pos[4])) cube.writeLED(pos[  ((i>>1)&1)*3],p,pos[2+(i&1)*3],HIGH);
      if ((pos[2]<=p)&&(p<=pos[5])) cube.writeLED(pos[  ((i>>1)&1)*3],pos[1+(i&1)*3],p,HIGH);
    }
  }
  cube.update();
}

void EF_GROWFRAME::scaleFrame(){
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
}

bool EF_GROWFRAME::step(){
  //the cube is defined by two points
  //initialize cordinates for two points
  if(tick%4==0){
    cube.clearPattern();
    if(phase<8){
      if((tick>>2)%13==0){
        //use phase to set dir[0] through dir[6]
        for(int i=0; i<3; i++){
          if( ( (phase>>i)&1 )==1){
            dir[i]=0;
            dir[i+3]=-1;
          }
          else{
            dir[i]=1;
            dir[i+3]=0;
          }
        }
      }

      if((tick>>2)%13==12) phase++;
      drawFrame();
      scaleFrame();
    }
    else {
      //okay now make the whole thing collapse in and out
      //dir[0]==0 should only ever be true immediately as we enter this keyframe
      if(dir[0]==0){
        for(int i=0; i<3; i++){
        dir[i]=1;
        dir[i+3]=-1;
        }
      }
    drawFrame();
    scaleFrame();
    if (tick>>2>=120) return(true);
    }
  }
  tick++;
  return(false);
}
