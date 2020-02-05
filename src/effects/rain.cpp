#include "rain.h"
#include <Arduino.h>
#include "../digitalWriteFast.h"
#include "../ledcube.h"


bool EF_RAIN::step(){
  if((tick%3)==0){
    //move the whole cube buffer down a voxel
    for (int y=0; y<8; y++){
      for(int z=0; z<7; z++){
        cube.pattern[y][z]=cube.pattern[y][z+1];
      }
      //top floor becomes empty
      cube.pattern[y][7]=0;
    }
    //if we're phase 1, light a random voxel on the top floor
    if ((phase==0)||(!play)) {
      cube.writeLED(random(0,8),random(0,8),7,HIGH);
      cube.writeLED(random(0,8),random(0,8),7,HIGH);
    }
    if ((tick/3>=250)&&(play)) phase++;
    if (phase>9)return(true);
    cube.update();
  }
  tick++;
  return(false);
}