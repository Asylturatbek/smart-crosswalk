#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Asyl32x16Font.h>
#include <fonts/Arial_Black_16.h>

SoftDMD dmd(1,2);  // DMD controls the entire display
DMD_TextBox box(dmd, 0, 0, 32, 16);  

int led = 10;
static boolean output = HIGH;

unsigned long timer=0;
int sys_active = 0;
int number = 25;


ISR(TIMER0_COMPA_vect){
  
  if (sys_active) {

    if(timer%1000==0){
      if (number<10) {
        dmd.clearScreen();
        dmd.drawString(7, 0, String(number));
      } else {
        dmd.clearScreen();
        dmd.drawString(-1, 0, String(number));
      }
      
      number--;
    }

    if(timer%300==0){
      digitalWrite(led, output);
      output = !output;
    }
    
    timer++;

    if (timer%2000==0){
      sys_stop();
    }
  }

}

void setup() {
  Serial.begin(9600);

  pinMode(led, OUTPUT);

  attachInterrupt(0, sys_start, RISING);

  cli();
  TCCR0A=(1<<WGM01);    //Set the CTC mode   
  OCR0A=0xF9; //Value for ORC0A for 1ms 
  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
  TCCR0B|=(1<<CS01);    //Set the prescale 1/64 clock
  TCCR0B|=(1<<CS00);
  sei();
  Serial.println("Timer0 Setup finished");
  
  dmd.setBrightness(100);
  dmd.selectFont(Asyl32x16Font);
  dmd.begin();
  dmd.clearScreen();
}



void loop() {
}

void sys_start(){
  sys_active = true;
}

void sys_stop(){
  sys_active = false;
  digitalWrite(led, LOW);
  output = HIGH;
  dmd.clearScreen();
  number = 25;
}
