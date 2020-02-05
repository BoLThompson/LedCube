#include "randomFill.h"
#include <Arduino.h>
#include "../digitalWriteFast.h"
#include "../ledcube.h"

EF_RANDOMFILL::EF_RANDOMFILL(){
  for(int i=0; i<512; i++){
    ledTrack[i]=false;
  }
}

bool EF_RANDOMFILL::step(){
  int ii;

  //pick a random number we haven't hit yet
  do {
    ii=random(513);
  }while(ledTrack[ii]==setval);

  //mark that number as one we've hit
  ledTrack[ii]=setval;

  //write that LED
  cube.writeLED(ii%8, (ii>>3)%8, (ii>>6)%8, setval);

  //add one to our animation ticks
  tick++;

  cube.update();

  //if we've hit all frames of the animation:
  if(tick==512){

    //add one to the phase
    phase++;

    //if we're done with phases 0 and 1, we're finished with a blank cube
    if (phase==2) return(true);

    //reverse direction
    setval=!setval;

    //start anew
    tick=0;
  }

  return(false);
}
