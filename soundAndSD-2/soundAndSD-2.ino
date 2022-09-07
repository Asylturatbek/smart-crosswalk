#include <SD.h>                           //include SD module library
#include <TMRpcm.h>                       //include speaker control library

#define SD_ChipSelectPin 10              //define CS pin

TMRpcm tmrpcm; //crete an object for speaker library

int led = 7;
int sys_active = 0;
unsigned long timer=0;
static boolean output = HIGH;

ISR(TIMER1_COMPA_vect){
//  if(sys_active) {
//    timer++;
//
//    if (timer%5000==0){
//      sys_stop();
//    }
//
//    if(timer%300==0){
//      digitalWrite(led, output);
//      output = !output;
//    }
//  }
  timer++;
  if(timer%300==0){
    digitalWrite(led, output);
    output = !output;
  }
}

void setup(){
  pinMode(led, OUTPUT);

  attachInterrupt(0, sys_start, RISING);

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 0xF9;
  TCCR1B = (1<<WGM12) | (1<<CS11);
  TCCR1B = (1<<CS10);
  TIMSK1 = (1<<OCIE1A);  
  
//  TCCR1A = 0;
//  TCCR1B=(1<<WGM12);    //Set the CTC mode   
//   //Value for ORC0A for 1ms 
//  TIMSK1|=(1<<OCIE1A);   //Set the interrupt request
//  TCCR1B|=();    //Set the prescale 1/64 clock
//  TCCR1B|=(1<<CS10);
  sei();
  Serial.println("Timer1 Setup finished");
  
  tmrpcm.speakerPin = 9; 
  tmrpcm.quality(1);//define speaker pin. 
                                          
  if (!SD.begin(SD_ChipSelectPin)) {      //see if the card is present and can be initialized
    
    return;                               //don't do anything more if not
  }
  
  tmrpcm.setVolume(6);                    //0 to 7. Set volume level
  tmrpcm.play("3.wav");         //the sound file "1" will play each time the arduino powers up, or is reset
}

void loop(){}

void sys_start(){
  sys_active = true;
//  output = HIGH;
//  digitalWrite(led, output);
//  output = !output;
}

void sys_stop(){
  sys_active = false;
  digitalWrite(led, LOW);
}
