#include <SPI.h>
#include <RF24.h>
#include "DeviceConfig.h"
#include "Statistic.h"
#include "pitches.h"

// times in seconds
#define SYS_INTERVAL               15     // crossing time for pedestrians
#define WAIT_INTERVAL              15     // for cooldown
#define STAND_STILL_INTERVAL       3     // waiting cars to stop

RF24 radio(PIN_RF_CE, PIN_RF_CSN); // CE, CSN
const byte addresses[][6] = { "node1", "node2" };

const unsigned char* address2 = addresses[0];; // change from adress2 to 1 when other box
const unsigned char* address1 = addresses[1];; // change """""
char go_message[5]   = "10315";
char wait_message[5] = "00000";
bool sys_active  = false;
bool report      = false;

int n_of_tx_tries           = 0;
int sys_countdown           = 0;
int wait_countdown          = 0;
int stand_still_countdown   = 0;

bool bEvent1Hz         = false;
bool bSysButtonPressed = false;
bool bRFReceived       = false;
bool wait_started      = false;

unsigned long timer=0;
unsigned char ButtonPressGuardCountDown = 0;
int payload = 0;   // waitseconds

ISR(TIMER4_COMPA_vect){  //This is the interrupt request
  
  if(sys_active) {
    if(timer == 0 && stand_still_countdown == 0){
      digitalWrite(PIN_RELAY_PROJECTOR, HIGH);
      if (sys_countdown == 0) {
        sys_stop(); return;
      }
      sys_countdown--; 
    }

    if ((timer & 0x01) == 0){                                      //This timer is 
      static boolean output = HIGH;
      static boolean output2 = HIGH;
      digitalWrite(PIN_RELAY_BLINKING, output);
      digitalWrite(PIN_RELAY_BLINKING2, output2);
      output = !output;
      //output2 = !output2;
    }
    
    timer++;
    if (timer == 8) {
      timer = 0;
      if (stand_still_countdown)
        stand_still_countdown--;
    }
      
  }
}

ISR(TIMER3_COMPA_vect){  //This is the interrupt request
  bEvent1Hz = true;
}

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
  Serial.println("\r\nInitialization...");

//  int some1 = 0;
//  int some2 = 14;
//  int some3 = 19;
//  int some4 = 25;
//  Serial.println(sizeof(some1));
//  Serial.println(sizeof(some2));
//  Serial.println(sizeof(some3));
//  Serial.println(sizeof(some4));

  pinMode(PIN_RELAY_PROJECTOR, OUTPUT);
  pinMode(PIN_RELAY_BLINKING,  OUTPUT);
  pinMode(PIN_RELAY_BLINKING2, OUTPUT);
  pinMode(PIN_RADIO_STATUS,    OUTPUT);
  pinMode(PIN_BUTTON_SYS,      INPUT);

  Serial.print("Radio: ");
  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  if (radio.getDataRate() == RF24_2MBPS) {
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(0x6f);
    radio.openWritingPipe(address1); // 00002
    radio.openReadingPipe(1, address2); // 00001
    radio.setPALevel(RF24_PA_HIGH);
    radio.startListening();
    Serial.println("OK");
  } else {
    ucBootResultFlags |= BOOT_RESULT_FLAG_RF_ERROR;
    Serial.println("Fail");
    digitalWrite(PIN_RADIO_STATUS, HIGH);
  }
  
  cli();
  
  // Initialize Timer4 for 8 Hz interrupt
  TCCR4A = 0;
  TCCR4B = 0;
  OCR4A = 1953;
  TIMSK4 &= ~(1 << OCIE4A);

  // Initialize Timer3 for 1 Hz interrupt
  TCCR3A = 0;
  TCCR3B = 0;
  OCR3A = 15625;
  TCCR3B = (1<<WGM32) | (1<<CS30) | (1<<CS32); // Set the prescale 1/1024 clock
  TIMSK3 = (1<<OCIE3A);
  Serial.println("Timer: OK");
  sei();
  
  EIFR = 0xFF; // clear all pending external interrupt ✅
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_SYS), sys_start, RISING);
  Statistic_Init();
  
  Serial.println("Program starts!");
}

void loop() {

    
  if ((ucBootResultFlags & BOOT_RESULT_FLAG_RF_ERROR) == 0) {
    while (radio.available()){
      radio.read(&payload, sizeof(payload));
      Serial.print("payload: ");
      Serial.println(payload);
      if (sys_active==false) { 
        bRFReceived = true;
        report = true;
        wait_countdown = payload;
      }
    }
  }
    
  if (!sys_active) {

    if ( (bSysButtonPressed) && (!bRFReceived) && (!report) ) {
        if ((ucBootResultFlags & BOOT_RESULT_FLAG_RF_ERROR) == 0) {
            radio.stopListening();
            TCNT3 = 0;  // set timer value to 0 to get values
            payload = wait_countdown;
            while( (!report) && (n_of_tx_tries<5) ) {
              report = radio.write(&payload, sizeof(payload));
              n_of_tx_tries++;
            }
              
            radio.startListening();
            
            if (report) {
              Serial.print("Reached, N 0f tries: ");
              Serial.println(n_of_tx_tries);
            } else {
              Serial.println("Couldn't reach the receiver!");
              bSysButtonPressed = false;
            }
            n_of_tx_tries = 0;
        }
    }
    
    bool signal_report = ( (bSysButtonPressed) || (bRFReceived) && (report) );
    
    if ( (signal_report) && (!wait_countdown) ) {
    
      Serial.println("Started the normal work!;");
      n_of_tx_tries = 0;
      report = false;
      
      bSysButtonPressed = false;
  
      sys_countdown = SYS_INTERVAL;
      stand_still_countdown = STAND_STILL_INTERVAL;
      Statistic_ButtonActivate();
      TCNT4=1950;//reset timer
      TCCR4B = (1<<WGM42) | (1<<CS40) | (1<<CS42); // Set the prescale 1/1024 clock
      TIMSK4 = (1<<OCIE4A); 
          
      Serial1.print(go_message);                 //  x xx xx format         X          XX             XX       FORMAT
      sys_active = true;                          //wait or go   stand_inter    go_interval
      Serial.println("Here in go!");

      bRFReceived = false;
      
    } else if ( (signal_report) && (wait_countdown) && (!wait_started) ) {
        wait_countdown = payload;
        wait_started = true;
        TCNT3=0;               //reset timer
        Serial1.print(wait_message);                //  x xx xx format         X          XX             XX       FORMAT
        Serial.println("Waiting started!");                     //wait or go   stand_inter    go_interval
    }
  }
  
  if (bEvent1Hz) {
    if(wait_countdown)
      wait_countdown--;
    
    bEvent1Hz = false;
    Statistic_1HzHook();
  }
  
}
  
void sys_start(){
  if(!sys_active) {
    Serial.print("pressed");
    bSysButtonPressed = true;
  }
  
}

void sys_stop() {
  TCCR4B = 0;
  TIMSK4 = 0;  
  
  sys_active = false;
  wait_started = false;
  wait_countdown = WAIT_INTERVAL;
  
  digitalWrite(PIN_RELAY_BLINKING, LOW);
  digitalWrite(PIN_RELAY_BLINKING2, LOW);
  digitalWrite(PIN_RELAY_PROJECTOR , LOW);
}
