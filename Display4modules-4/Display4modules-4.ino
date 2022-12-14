#include <SPI.h>
#include <DMD2.h>
#include "ChangedRus5x7.h"
#include <fonts/Asyl32x16Font.h>  

SoftDMD dmd(1,4);  // DMD controls the entire display
DMD_TextBox box1(dmd, 0, 0, 32, 16);  
DMD_TextBox box2(dmd, 0, 16, 32, 32); 
DMD_TextBox box3(dmd, 0, 49, 32, 16); 

int go_countdown   = 0;
int stand_still_countdown = 0;

unsigned long timer = 0;
int countdown = 0;
bool sys_active = false;
bool should_update = false;
bool stand_still = false;

char text[10];
byte chPos = 0;
byte ch = 0;

ISR(TIMER2_COMPA_vect){  //This is the interrupt request

  if(sys_active){
    timer++;
    if ((timer) == 100) {    //for 125hz to make 1 sec

      should_update = true;

      timer = 0;             // RESETTING THE TIMER VALUE
      
    }

  }
}

void setup() {
  Serial.begin(9600);

  // TIMER 0 for interrupt frequency 125 Hz:
  cli(); // stop interrupts
  TCCR2A = 0; // set entire TCCR2A register to 0
  TCCR2B = 0; // same for TCCR2B
  TCNT2  = 0; // initialize counter value to 0
  TCCR2A = 1<<WGM21;    //Set the CTC mode 
  OCR2A = 0x9C; // 100hz
  TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20); // Set CS02, CS01 and CS00 bits for 1024 prescaler
  //  TIMSK2 |= (1 << OCIE2A);// enable timer compare interrupt
  TIMSK2 &= ~(1 << OCIE2A);
  sei(); // allow interrupts
  
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

    if (chPos == 5) {
      Serial.println(text);
      int x = text[0] - '0';
      if (x == 0) {
        sys_stand_still();
      } else if (x == 1) {
        int sfirst  = text[1] - '0';
        int ssecond = text[2] - '0';
        
        int gfirst  = text[3] - '0';
        int gsecond = text[4] - '0';

        stand_still_countdown = (sfirst * 10) + ssecond;
        go_countdown          = (gfirst * 10) + gsecond;

        sys_start_go();
        Serial.println(stand_still_countdown);
        Serial.println(go_countdown);
      }
    }
    
    chPos = 0;   // restarting the character position

  } 


  if (should_update) {
    update_screen();
    should_update = false;
  }
   
   
}

void sys_stand_still(){  // ждите старт, duration controlled by Mega
  
    Serial.println("sys stand still!");
    dmd.selectFont(ChangedRus5x7);
    dmd.drawString(1, 20, ";LBNT");  // ждите
    dmd.drawString(1, 36, "CNFHN");  // старт
    
}

void sys_start_go(){

    Serial.println("sys start go!"); 
    clearAllScreen();
    countdown = stand_still_countdown;
    stand_still = true; // show red countdown
    
    sys_active = true;
    timer = 97;          //value to start immediately
    TCNT2 = 0;            //reset timer
    TIMSK2 = (1<<OCIE0A); //start timer
  
}

void sys_stop(){
  TIMSK2 = 0;

  go_countdown          = 0;
  stand_still_countdown = 0;
  
  sys_active = false;
  clearAllScreen();
}

void update_screen(){

  dmd.selectFont(ChangedRus5x7);
  
  if (stand_still) {
    
    if (countdown == stand_still_countdown)
      Serial.println("updating the stop text!");
      dmd.drawString(4, 4, "CNJG");   // стоп

    if(countdown==0) {
      stand_still = false; countdown = go_countdown; box1.clear();}
      
  }
  
  if (!stand_still) {

    if (countdown == go_countdown)
      Serial.println("updating the go text!");
      box3.print("GHJ[J-LBNT");     //проходите

    if(countdown==0){
      sys_stop(); box3.clear(); return;}
    
  }

  box2.clear();
  dmd.selectFont(Asyl32x16Font);

  if (countdown<10)
      dmd.drawString(7, 16, String(countdown));
    else
      dmd.drawString(-1, 16, String(countdown));

  countdown--;
  
}

void clearAllScreen(){
  box1.clear();
  box2.clear();
  box3.clear();
}
