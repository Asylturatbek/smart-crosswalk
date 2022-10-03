#include "pitches.h"
int speaker = 9;
int led = 7;

int sys_active = 0;
unsigned long timer=0;
unsigned long BuzzerIntervalCount = 0;
static boolean output = HIGH;

ISR(TIMER1_COMPA_vect){  //This is the interrupt request
  if(sys_active){
    if ((timer & 0x07) == 0) {
      tone(speaker, NOTE_C5, 200);
    } else if ((timer & 0x07) == 2) {
     tone(speaker, NOTE_FS4, 200);
     BuzzerIntervalCount--;
     if (BuzzerIntervalCount == 0)
     {
      sys_active = false;
     }
    }
    timer++;

  }
  
  
}

void setup() {
  Serial.print("Initialising...");
  cli();
    
  attachInterrupt(0, sys_start, FALLING);
  
  Serial.begin(9600);

  // Setting up timer1 for 8Hz interrupt
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 1953;
  TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); // Set the prescale 1/1024 clock
  TIMSK1 = (1<<OCIE1A);  
  sei();
  Serial.println("OK");
}

void loop() {
//  if ((timer2 & 0x28) == 0) {
//      sys_active = false;
//  }
}

void sys_start(){
  if (sys_active == false)
    BuzzerIntervalCount = 5;
  sys_active = true;
}

void sys_stop(){
  sys_active = false;
}
