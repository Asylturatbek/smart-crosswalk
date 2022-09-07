#include <SD.h>                           //include SD module library
#include <TMRpcm.h>                       //include speaker control library

#define SD_ChipSelectPin 10              //define CS pin

TMRpcm tmrpcm; //crete an object for speaker library

int led = 7;
int sys_active = 0;
unsigned long timer=0;
static boolean output = HIGH;

void setup(){
  pinMode(led, OUTPUT);

  attachInterrupt(0, sys_start, RISING);
  attachInterrupt(1, sys_stop, RISING);
  
  tmrpcm.speakerPin = 9; 
  tmrpcm.quality(1);//define speaker pin. 
                                          
  if (!SD.begin(SD_ChipSelectPin)) {      //see if the card is present and can be initialized
    return;                               //don't do anything more if not
  }
  
  tmrpcm.setVolume(6);                    //0 to 7. Set volume level
          //the sound file "1" will play each time the arduino powers up, or is reset
}

void loop(){}

void sys_start(){
  sys_active = true;
  digitalWrite(led, LOW);
  tmrpcm.play("3.wav"); 
}

void sys_stop(){
  sys_active = false;
  digitalWrite(led, LOW);
  tmrpcm.stopPlayback();
}
