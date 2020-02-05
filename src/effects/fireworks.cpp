#include "fireworks.h"
#include <Arduino.h>
#include "../digitalWriteFast.h"
#include "../ledcube.h"

EF_FIREWORK::EF_FIREWORK(){
  setrocket();
}

EF_FIREWORK::~EF_FIREWORK(){
  //clean up sparkybois
  if (phase==2) killsparks();
  delete[] sparks;
}

void EF_FIREWORK::killsparks(){
  for (int i=0; i<sparkcount; i++)
    delete sparks[i];
}

void EF_FIREWORK::setrocket(){
  //pick a starting coordinate on the bottom floor
  pos[0]=(byte)random(2,6);
  pos[1]=(byte)random(2,6);
  pos[2]=-1;
  fuse=random(3,6);
}

bool EF_FIREWORK::step(){

  //phase 2: spark processing
  if (phase==2){
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
    if (tick>64){
      if (shot==0) return(true);
      shot--;
      killsparks();
      setrocket();
      phase=0;
      tick=0;
    }
  }
  
  //phase 1: rocket freezes (and then releases the sparkybois)
  if ((phase==1)&&(tick%8==0)){
    phase=2;
    tick=0;
    for (byte i=0; i<sparkcount; i++){
      sparks[i]=new EF_FW_SPARK;
      sparks[i]->setPosition(pos[0],pos[1],pos[2]);
      sparks[i]->setDirection(0.628*(float)random(0,11),0.628*(float)random(0,11));
      sparks[i]->setSpeed(random(8,16));
      sparks[i]->setGravity(.1);
    }
    cube.update();
  }

  //phase 0: rocket ascends
  if ((phase==0)&&(tick%4==0)){
    //erase old position
    cube.writeLED(pos[0],pos[1],max(0,pos[2]),LOW);

    pos[2]++;
    if (fuse>0){
    fuse--;

    //write new position
    cube.writeLED(pos[0],pos[1],pos[2],HIGH);
    }
    else {
      phase++;
      tick=0;
    }
  cube.update();
  }

  tick++;
  return(false);
}

void EF_FW_SPARK::setDirection(int parg, int yarg){
  pitch=parg;
  yaw=yarg;
}

void EF_FW_SPARK::setSpeed(byte arg){
  speed=arg;
}

void EF_FW_SPARK::step(){
  //cosine=opposite over hypoteneuse
  //sine=opposite over hypoteneuse
  //tangent=opposite over adjacent

  //later on we'll need to account for Z stuff

  //take the angle
  //take our speed
  //multiply the cosine of our angle with the hypoteneuse to find the opposite (y)
  //sine*speed=x

  int zspeed=speed*sin(pitch);
  int flatspeed=speed*cos(pitch);

  int xspeed=flatspeed*sin(yaw);
  int yspeed=flatspeed*cos(yaw);

  x+=xspeed;
  y+=yspeed;
  z+=zspeed;

  gravmomentum+=gravity;
  z=max(0,z-round(gravmomentum));

  speed=max(speed-1,0);
}

void EF_FW_SPARK::setGravity(float arg){
  gravity=arg;
}

void EF_FW_SPARK::draw(){
  //light an LED according to round((int)position/8)
  //ledcube.writeLED(x>>3,y>>3,z>>3,HIGH);
}

byte EF_FW_SPARK::getX(){
  return(max(0,min(7,((byte)x)>>4)));
}

byte EF_FW_SPARK::getY(){
  return(max(0,min(7,((byte)y)>>4)));
}

byte EF_FW_SPARK::getZ(){
  return(max(0,min(7,((byte)z)>>4)));
}

// void EF_FW_SPARK::erase(){
//
// }

EF_FW_SPARK::EF_FW_SPARK() {
}

EF_FW_SPARK::EF_FW_SPARK(byte xarg, byte yarg, byte zarg){
  x=xarg<<4;
  y=yarg<<4;
  z=zarg<<4;
}

bool EF_FW_SPARK::onField(){
  return ((0<x)&&(x<255)&&(0<y)&&(y<255)&&(0<z)&&(z<255));
}

void EF_FW_SPARK::setPosition(byte xarg, byte yarg, byte zarg){
  x=xarg<<4;
  y=yarg<<4;
  z=zarg<<4;
}
