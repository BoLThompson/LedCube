#include "planemove.h"
#include <Arduino.h>
#include "../digitalWriteFast.h"
#include "../ledcube.h"

EF_PLANEMOVE::EF_PLANEMOVE(){
  for(int i=0; i<3; i++){
    pos[i]=0;
  }
}

bool EF_PLANEMOVE::step(){
  if (tick%4==0){
    cube.clearPattern();
    //do each of the axes as determined by phase
    //start forwards
    // for(pos[phase%3]=0; pos[phase%3]+dir>=0; pos[phase%3]+=dir){
    //draw the plane
    for(pos[(phase+1)%3]=0; pos[(phase+1)%3]<8; pos[(phase+1)%3]++){
      for(pos[(phase+2)%3]=0; pos[(phase+2)%3]<8; pos[(phase+2)%3]++){
        cube.writeLED(pos[0],pos[1],pos[2],HIGH);
      }
    }
    pos[phase%3]+=dir;
    //flip directions
    cube.update();
    if ((pos[phase%3]==8)||(pos[phase%3]==-1)){
      pos[phase%3]-=dir;
      dir*=-1;
      if (((phase>2)&&(pos[phase%3]==7))||((phase==2)&&(pos[phase%3]==0))){
        pos[0]=7;
        pos[1]=7;
        pos[2]=7;
        dir=-1;
        phase++;
      }
      else if ((phase<2)&&(pos[phase%3]==0)){
        pos[0]=0;
        pos[1]=0;
        pos[2]=0;
        dir=1;
        phase++;
      }
      else pos[phase%3]+=dir;
    }
    if (phase==6) return(true);
  }
  tick++;
  return(false);
}