#include "pitches.h"
int speaker = 9;
int led = 7;

int sys_active = 0;
unsigned long timer=0;
unsigned long timer2 = -200;
static boolean output = HIGH;

ISR(TIMER1_COMPA_vect){  //This is the interrupt request
  
  if(sys_active) {

    if(timer%500==0){
      tone(speaker, NOTE_C5, 200);
      Serial.println("firstPhase");
      Serial.println(timer);
    }

    

    if(timer2%500 == 0){
      tone(speaker, NOTE_FS4, 200);
      Serial.println("secondPhase");
      Serial.println(timer);
    }

    timer++;
    timer2++;
    

    if (timer%5000==0){
      sys_stop();
    }
  }
  
}

void setup() {
  attachInterrupt(0, sys_start, FALLING);
  Serial.begin(9600);
//
//  cli();
//  TCCR0A=(1<<WGM01);    //Set the CTC mode   
//  OCR0A=0xF9; //Value for ORC0A for 1ms 
//  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
//  TCCR0B|=(1<<CS01);    //Set the prescale 1/64 clock
//  TCCR0B|=(1<<CS00);
//  sei();
//  Serial.println("Timer0 Setup finished");

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 0xF9;
  TCCR1B = (1<<WGM12) | (1<<CS11);
  TCCR1B = (1<<CS10);
  TIMSK1 = (1<<OCIE1A);  
  sei();
  Serial.println("Timer1 Setup finished");
}

void loop() {
  
}

void sys_start(){
  sys_active = true;
}

void sys_stop(){
  sys_active = false;
}
