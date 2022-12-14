#include "pitches.h"

#define OUTPUT_STATUS_PERIOD  2   // in sec

int speaker = 9;
int led = 7;
int sys_active = 0;
unsigned char timer=0;
unsigned char BuzzerAlertCount = 0;
static boolean output = HIGH;
unsigned char ucOutputStatusCount = OUTPUT_STATUS_PERIOD;
boolean b1HzEvent = false;

ISR(TIMER1_COMPA_vect){  //This is the interrupt request
  if ((timer & 0x07) == 0)
  {
    b1HzEvent = true;
    if(sys_active)
    {
      tone(speaker, NOTE_C5, 200);
    }
  }
  else if ((timer & 0x07) == 2)
  {
    if(sys_active)
    {
      tone(speaker, NOTE_FS4, 200);
      BuzzerAlertCount--;
      if (BuzzerAlertCount == 0)
      {
        sys_active = false;
      }
    }
  }
  timer++;
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
  if (b1HzEvent)
  { // 1Hz Handler
    b1HzEvent = false;
    
  }
}

void sys_start(){
  if (sys_active == false)
    BuzzerAlertCount = 6;
  sys_active = true;
}

void sys_stop(){
  sys_active = false;
}
