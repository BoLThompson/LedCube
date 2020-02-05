#include <Arduino.h>
#include "digitalWriteFast.h"
#include "ledcube.h"
#include "effects/fireworks.h"
#include "effects/randomfill.h"
#include "effects/growFrame.h"
#include "effects/rain.h"
#include "effects/planeMove.h"
#include "FFT/arduinoFFT.h"

arduinoFFT FFT = arduinoFFT();
#define CHANNEL1 A6
#define CHANNEL2 A7 //lol stereo FFT
const uint16_t samples = 16; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 5000; //Hz, must be less than 10000 due to ADC
const double signalFrequency = 1000;
const uint8_t amplitude = 100;

unsigned int sampling_period_us;
unsigned long microseconds;


double vReal[samples];
double vImag[samples];
#define sampling_period_us round(1000000*(1.0/samplingFrequency))

#define pinStatus 3
#define pinSW1 17
#define pinSW2 18

const int effectCount=5;
const int visEffectCount=1;

int effectIndex=0;
byte dimness=0;
bool play=true;
volatile bool visualizer=false;
volatile bool skipflag=false;
bool sw1=false;
bool sw2=false;

LedCube cube;

const uint16_t T0_COMP = 81250;

void setup() {
  Serial.begin(115200);
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

  bool curSw1=digitalReadFast(pinSW1);
  bool curSw2=digitalReadFast(pinSW2);

  //At this point, curSwX is the state of the switch and swX is the last known state of the switch
  //using these two variables together we can determine whether the state of the switch just changed.

  #define sw1Pressed (curSw1&&!sw1)
  #define sw2Pressed (curSw2&&!sw2)

  //if a button is pressed while the other is held
  if ((sw1Pressed&&curSw2)||(sw2Pressed&&curSw1)){
    //go into music visualization mode
    effectIndex=0;
    play=true;
    skipflag=true;
    visualizer=true;
    digitalWriteFast(pinStatus,HIGH);
    pinMode(pinStatus,OUTPUT);
  }
  //otherwise, if sw1 just got pressed
  else if (sw1Pressed){
    //unpause and proceed to the next effect
    play=true;
    pinMode(pinStatus,INPUT);
    skipflag=true;
    visualizer==false;
  }
  else if ((sw2Pressed)&&(!visualizer)){
    //pause
    play=false;
    digitalWriteFast(pinStatus,LOW);
    pinMode(pinStatus,OUTPUT);
    visualizer==false;
  }
  sw1=curSw1;
  sw2=curSw2;
}

void playEffect(EFFECT& effect){
  bool proceed;
  do{
    //returns false until the effect has completed
    proceed=effect.step();
    //push this cube update and wait for it to display
    cube.waitForFrame();
  }
  while ((!proceed)&&(!skipflag));
  //skipflag is set true during a timer interrupt if the skip button is freshly pressed
  skipflag=false;

  cube.clearPattern();

  if (play) effectIndex++;
  int curmax = effectCount;
  if (visualizer) curmax = visEffectCount;
  if (effectIndex==curmax) effectIndex=0;
}

void loop() {
  //create an effect object
  while (!visualizer){
    switch(effectIndex){
    case 0:
      {
        EF_PLANEMOVE effect;
        playEffect(effect);
      }
      break;
    case 1:
      {
        EF_RANDOMFILL effect;
        playEffect(effect);
      }
      break;
    case 2:
      {
        EF_GROWFRAME effect;
        playEffect(effect);
      }
      break;
    case 3:
      {
        EF_RAIN effect;
        playEffect(effect);
      }
      break;
    case 4:
      {
        EF_FIREWORK effect;
        playEffect(effect);
      }
      break;
    }
  }

  effectIndex=0;
  skipflag=false;
  while (visualizer) {
    while(!skipflag){
      /*SAMPLING*/
      double cycles = (((samples-1) * signalFrequency) / samplingFrequency); //Number of signal cycles that the sampling will read
      for (uint16_t i = 0; i < samples; i++)
      {
        vReal[i] = (analogRead(CHANNEL1)>>1)+(analogRead(CHANNEL2)>>1);/* Build data with positive and negative values*/
        vImag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
      }
      FFT.Compute(vReal, vImag, samples, FFT_FORWARD); // Compute FFT 
      FFT.ComplexToMagnitude(vReal, vImag, 8); // Compute magnitudes 


      //move the whole cube buffer back a voxel
      for (int z=0; z<8; z++){
        for(int y=8; y>0; y--){
          cube.pattern[y][z]=cube.pattern[y-1][z];
        }
        //top floor becomes empty
        cube.pattern[0][z]=0;
      }

      //get some do
      int out=0;
      for (int i=0; i<8; i++){
        out = (int) floor(sqrt(    sqrt(vReal[i+1]*vReal[i+1]+vImag[i+1]*vImag[i*1])    ));
        out>>=1;
        out--;
        out=min(out,8);
        out=max(0,out);
        for(int ii=0; ii<out; ii++){
          cube.writeLED(i,0,ii,HIGH);
        }
      }
      cube.update();
      cube.waitForFrame();
    }
    //pinMode(pinStatus,INPUT);
    visualizer=false;
    pinMode(pinStatus,INPUT);
    effectIndex=0;
    play=true;
  }
}