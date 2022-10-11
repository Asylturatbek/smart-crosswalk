#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Asyl32x16Font.h>
#include <fonts/Arial_Black_16.h>

SoftDMD dmd(1,4);  // DMD controls the entire display
DMD_TextBox box(dmd, 0, 0, 32, 16);  

int led = 5;
static boolean output = HIGH;

unsigned long timer=0;
int sys_active_red = 0;
int sys_active_green = 0;
int countdown1 = 25;
int countdown2 = 25;


ISR(TIMER1_COMPA_vect){  //This is the interrupt request
  
  if(sys_active_red) {
    if((timer & 0x07) == 0){
      if (countdown1 == 0) {
        sys_stop();
        return;
      }
      else {
        dmd.clearScreen();
        if (countdown1<10)
          dmd.drawString(7, 0, String(countdown1));
        else
          dmd.drawString(-1, 0, String(countdown1));
        countdown1--;
      }
    }
    timer++;
  }

  if(sys_active_green) {
    if((timer & 0x07) == 0){
      if (countdown2 == 0) {
        sys_stop();
        return;
      }
      else {
        dmd.clearScreen();
        if (countdown2<10)
          dmd.drawString(7, 32, String(countdown2));
        else
          dmd.drawString(-1, 32, String(countdown2));
        countdown2--;
      }
    }
    timer++;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(led, OUTPUT);

  attachInterrupt(0, sys_start_RED, RISING);
  attachInterrupt(1, sys_start_GREEN, RISING);

  Serial.print("Initialising...");
  cli();// Setting up timer1 for 8Hz interrupt
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 1953;
  TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); // Set the prescale 1/1024 clock
  TIMSK1 = (1<<OCIE1A);  
//  TIMSK1 &= ~(1 << OCIE1A);
  sei();
  Serial.println("OK");
  
  dmd.setBrightness(150); // (0-255)
  dmd.selectFont(Asyl32x16Font);
  dmd.beginNoTimer();
  dmd.clearScreen();
}



void loop() {
  
}

void sys_start_RED(){
  sys_active_red = true;
//  TCNT1=1950;//reset timer
//  TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); // Set the prescale 1/1024 clock
//  TIMSK1 = (1<<OCIE1A); 
}

void sys_start_GREEN(){
  sys_active_green = true;
//  TCNT1=1950;//reset timer
//  TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); // Set the prescale 1/1024 clock
//  TIMSK1 = (1<<OCIE1A); 
}

void sys_stop(){
  sys_active_red = false;
  sys_active_green = false;
  dmd.clearScreen();
  countdown1 = 25;
  countdown2 = 25;
}
