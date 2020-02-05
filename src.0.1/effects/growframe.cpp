#include "growFrame.h"
#include <Arduino.h>
#include <digitalWriteFast.h>
#include "../ledcube.h"

void EF_GROWFRAME::drawandscale(){
  //draw the frame
  for(int p=8; p>=0; p--){
    for(int i=0; i<6; i++){
      if ((pos[0]<=p)&&(p<=pos[3])) cube->writeLED(p,pos[1+((i>>1)&1)*3],pos[2+(i&1)*3],HIGH);
      if ((pos[1]<=p)&&(p<=pos[4])) cube->writeLED(pos[  ((i>>1)&1)*3],p,pos[2+(i&1)*3],HIGH);
      if ((pos[2]<=p)&&(p<=pos[5])) cube->writeLED(pos[  ((i>>1)&1)*3],pos[1+(i&1)*3],p,HIGH);
    }
  }

  if(tick%4==3){
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
  }

  //the cube will scale as three of these values travel towards the opposite extreme, then move them back
}

EF_GROWFRAME::EF_GROWFRAME(LedCube *arg){
  cube=arg;
  for (int i=0; i<3; i++){
    pos[i]=0;
    pos[i+3]=7;
  }
}

bool EF_GROWFRAME::step(){
  cube->clearPattern();
  if (tick<100){
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
        drawandscale();
      }
    }
  }
  //okay now make the whole thing collapse in and out
  // for(int i=0; i<3; i++){
  //   dir[i]=1;
  //   dir[i+3]=-1;
  // }
  //
  // for (int i=0; i<4; i++){
  //   for(int keyframe=0; keyframe<8; keyframe++){
  //     drawandscale(pos,dir);
  //   }
  // }
  tick++;
  return(false);
}
