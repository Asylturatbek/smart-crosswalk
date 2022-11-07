#include <SPI.h>
#include <DMD2.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/Asyl32x16Font.h>

#define GO_COUNTDOWN              5  // in sec
#define WAIT_COUNTDOWN              5  // in sec

SoftDMD dmd(1,4);  // DMD controls the entire display
DMD_TextBox box(dmd, 0, 0, 32, 16);  

unsigned long timer=0;
int sys_active = 0;
bool should_go = 0;
bool should_wait = 0;
bool should_stand_idle = 0;

int countdown = 0;

ISR(TIMER0_COMPA_vect){  //This is the interrupt request

  if(sys_active){
    timer++;
    if ((timer) == 124) { //for 125hz to make 1 sec
      
      show_display();

      timer = 0; // RESETTING THE TIMER VALUE
      
    } 

  }
}

void setup() {
  Serial.begin(9600);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  attachInterrupt(1, sys_start_wait, FALLING);
  attachInterrupt(0, sys_start_go, FALLING);

  // TIMER 0 for interrupt frequency 125 Hz:
  cli(); // stop interrupts
  TCCR0A = 0; // set entire TCCR0A register to 0
  TCCR0B = 0; // same for TCCR0B
  TCCR0A=(1<<WGM01);    //Set the CTC mode 
  TCNT0  = 0; // initialize counter value to 0
  // set compare match register for 125 Hz increments
  OCR0A = 124; // = 16000000 / (1024 * 125) - 1 (must be <256)
  TCCR0B |= (1 << CS02) | (0 << CS01) | (1 << CS00); // Set CS02, CS01 and CS00 bits for 1024 prescaler
//  TIMSK0 |= (1 << OCIE0A);// enable timer compare interrupt
  TIMSK0 &= ~(1 << OCIE0A);
  sei(); // allow interrupts

  
//  TCCR4B = (1<<WGM42) | (1<<CS40) | (1<<CS42); // Set the prescale 1/1024 clock
   
  
  dmd.setBrightness(100);
  dmd.selectFont(Asyl32x16Font);
  dmd.begin();
  dmd.clearScreen();

  dmd.selectFont(SystemFont5x7);
  dmd.drawString(4, 5, String(55));
}



void loop() {
}

void sys_start_wait(){
  if (!sys_active){
    dmd.clearScreen();
    dmd.selectFont(SystemFont5x7);
    dmd.drawString(4, 5, String("Wait"));
    dmd.drawString(4, 15, String("Please"));
    dmd.drawString(4, 37, String("Until it"));
    dmd.drawString(4, 47, String("starts!"));
  }
  
}

void sys_start_go(){

  if(!sys_active){
    Serial.println("pressed wait ");
    countdown = WAIT_COUNTDOWN;
    should_wait = true;
    should_go = false;
    
    sys_active = true;
    timer = 123;
    TCNT0=120;//reset timer
    TIMSK0 = (1<<OCIE0A);
  }
  
}

void sys_stop(){
  TIMSK0 = 0;
  
  sys_active = false;
  should_wait = false;
  should_go = false;
  dmd.clearScreen();
}

void show_display(){
  
  dmd.clearScreen();
  dmd.selectFont(SystemFont5x7);
  
  if (should_wait) {
      
    dmd.drawString(4, 5, String("Wait"));
    dmd.selectFont(Asyl32x16Font);
    if (countdown<10)
      dmd.drawString(7, 32, String(countdown));
    else
      dmd.drawString(-1, 32, String(countdown));

    if(countdown==1) {
      should_wait = false; should_go = true; countdown = GO_COUNTDOWN; return;}
      
  } else if (should_go) {

    if(countdown==0){
      should_wait = false; should_go = false; sys_stop(); return;}
    
    dmd.drawString(4, 37, String("Go"));
    dmd.selectFont(Asyl32x16Font);
    if (countdown<10)
      dmd.drawString(7, 0, String(countdown));
    else
      dmd.drawString(-1, 0, String(countdown));
    
  }

  countdown--;
  
}
