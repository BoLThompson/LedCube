#include "fireworks.h"
#include <Arduino.h>

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
