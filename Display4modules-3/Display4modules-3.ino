#include <SPI.h>
#include <DMD2.h>
#include "ChangedRus5x7.h"
#include <fonts/Asyl32x16Font.h>

#define GO_COUNTDOWN                5  // in sec
#define WAIT_COUNTDOWN              5  // in sec

SoftDMD dmd(1,4);  // DMD controls the entire display
DMD_TextBox box1(dmd, 0, 0, 32, 16);  
DMD_TextBox box2(dmd, 0, 16, 32, 32); 
DMD_TextBox box3(dmd, 0, 49, 32, 16); 

unsigned long timer = 0;
int countdown = 0;
bool sys_active = false;
bool should_update = false;
bool should_go = false;
bool should_wait = false;
bool should_stand_idle = false;

char text[10];
byte chPos = 0;
byte ch = 0;

int output2 = LOW;
int output3 = LOW;

ISR(TIMER0_COMPA_vect){  //This is the interrupt request

  if(sys_active){
    timer++;
    if ((timer) == 124) {    //for 125hz to make 1 sec

      should_update = true;

      timer = 0;             // RESETTING THE TIMER VALUE
      
    }

  }
}

void setup() {
  Serial.begin(9600);

  
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  attachInterrupt(0, sys_start_wait, RISING);
  attachInterrupt(1, sys_start_go, RISING);
//  pinMode(2, OUTPUT);
//  pinMode(3, OUTPUT);

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
   
  
  dmd.setBrightness(170);
  dmd.selectFont(ChangedRus5x7);
  dmd.begin();
  clearAllScreen();

//  dmd.drawString(0, 0, String("QWERT"));
//  dmd.drawString(0, 8, String("YUIOP"));
//  dmd.drawString(0, 16, String("[]ASD"));
//  dmd.drawString(0, 32, String("FGHJK"));
//  dmd.drawString(0, 40, String("L;'ZX"));
//  dmd.drawString(0, 48, String("CVBNM"));
//  dmd.drawString(0, 56, String(",."));

  delay(100);
  
  dmd.drawString(4, 4, "CNJG");  //стоп
  box2.print(";LBNT YFXFKJ");    //ждите начало
  box3.print("GHJ[J-LBNT");      //прохо-дите
  Serial.println("Started!");
}



void loop() {

  if (Serial.available()){
    while(Serial.available()) {
       //Serial.println("there is new data");
      //read incoming char by char:
       ch = Serial.read();
       text[chPos] = ch;
       chPos++;
       delay(10);  
    }
    text[chPos] = 0; //terminate cString
    chPos = 0;
   
    Serial.println(text);

    if (text == "wait") {
      Serial.print("2");
//      toggle2();
    } else if (text == "go") {
      Serial.print("3");
//      toggle3();
    }
  } 


  if (should_update) {
    update_screen();
    should_update = false;
  }
   
   
}

void toggle2() {
  digitalWrite(2, output2);
  output2 = !output2;
}

void toggle3() {
  digitalWrite(3, output3);
  output3 = !output3;
}

void sys_start_wait(){  // ждите старт, duration controlled by Mega
  
  if (!sys_active){
    Serial.println("sys start wait!");
    dmd.selectFont(ChangedRus5x7);
    dmd.drawString(1, 20, ";LBNT");  // ждите
    dmd.drawString(1, 36, "CNFHN");  // старт
  }
}

void sys_start_go(){

  if(!sys_active){

    Serial.println("sys start go!"); 
    clearAllScreen();
    countdown = WAIT_COUNTDOWN;
    should_wait = true; // show red countdown
    should_go = false; // show green countdown
    
    sys_active = true;
    timer = 123;          //value to start immediately
    TCNT0=120;            //reset timer
    TIMSK0 = (1<<OCIE0A); //start timer
  }
  
}

void sys_stop(){
  TIMSK0 = 0;
  
  sys_active = false;
  should_wait = false;
  should_go = false;
  clearAllScreen();
}

void update_screen(){
  
  box2.clear();
  dmd.selectFont(Asyl32x16Font);

  if (countdown<10)
      dmd.drawString(7, 16, String(countdown));
    else
      dmd.drawString(-1, 16, String(countdown));

  dmd.selectFont(ChangedRus5x7);
  
  if (should_wait) {
    
    if (countdown == WAIT_COUNTDOWN)
      Serial.println("updating the stop text!");
      dmd.drawString(4, 4, "CNJG");   // стоп

    if(countdown==1) {
      should_wait = false; should_go = true; countdown = GO_COUNTDOWN; box1.clear(); return;}
      
  } else if (should_go) {

    if (countdown == GO_COUNTDOWN)
      Serial.println("updating the go text!");
      box3.print("GHJ[J-LBNT");     //проходите

    if(countdown==0){
      sys_stop(); box3.clear(); return;}
    
  }

  countdown--;
  
}

void clearAllScreen(){
  box1.clear();
  box2.clear();
  box3.clear();
}
