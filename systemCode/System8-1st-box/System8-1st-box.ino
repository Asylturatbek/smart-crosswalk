#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DeviceConfig.h"
#include "Statistic.h"
#include "pitches.h"

#define SYS_INTERVAL              25 
#define WAIT_INTERVAL              5// in sec
#define STAND_STILL_INTERVAL       5

RF24 radio(PIN_RF_CE, PIN_RF_CSN); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
const unsigned char* address2= addresses[0];;
const unsigned char* address1 = addresses[1];;
boolean signalState = 0;
int sys_active = 0;
unsigned long timer=0;
unsigned long sys_countdown;
int wait_countdown = 0;
int stand_still_countdown = 0;
boolean bEvent1Hz = false;
boolean bSysButtonPressed = false;
boolean bRFReceived = false;
unsigned char ButtonPressGuardCountDown = 0;
boolean should_wait = false;
boolean wait_started = false;


ISR(TIMER4_COMPA_vect){  //This is the interrupt request
  if(sys_active) {
    if(timer == 0 && stand_still_countdown == 0){
      digitalWrite(PIN_RELAY_PROJECTOR, HIGH);
      if (sys_countdown == 0) {
        sys_stop();
        return;
      } else {
        tone(PIN_SPEAKER, NOTE_C5, 200);
        sys_countdown--; 
      }
    }
    
    if (timer == 2 && stand_still_countdown == 0)
      tone(PIN_SPEAKER, NOTE_FS4, 200);

    if ((timer & 0x01) == 0){
      static boolean output = HIGH;
      static boolean output2 = LOW;
      digitalWrite(PIN_RELAY_BLINKING, output);
      digitalWrite(PIN_RELAY_BLINKING2, output2);
      output = !output;
      output2 = !output2;
    }
    
    timer++;
    if (timer == 8) {
      timer = 0;
      if (stand_still_countdown) {
        stand_still_countdown--;
      }
    }
      
  }
}

ISR(TIMER3_COMPA_vect){  //This is the interrupt request
  bEvent1Hz = true;
}

void setup() {
  Serial.begin(9600);
  Serial.println("\r\nInitialization...");

  pinMode(PIN_RELAY_PROJECTOR, OUTPUT);
  pinMode(PIN_RELAY_BLINKING, OUTPUT);
  pinMode(PIN_RELAY_BLINKING2, OUTPUT);
  pinMode(PIN_COUNTDOWN_RED, OUTPUT);
  pinMode(PIN_COUNTDOWN_GREEN, OUTPUT);
  pinMode(PIN_BUTTON_SYS, INPUT_PULLUP);
  stop_wait();
  stop_go();


  Serial.print("Radio: ");
  radio.begin();
  radio.setDataRate(RF24_2MBPS);
  if (radio.getDataRate() == RF24_2MBPS)
  {
    radio.setDataRate(RF24_1MBPS);
    radio.openWritingPipe(address1); // 00002
    radio.openReadingPipe(1, address2); // 00001
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
    Serial.println("OK");
  }
  else
  {
    ucBootResultFlags |= BOOT_RESULT_FLAG_RF_ERROR;
    Serial.println("Fail");
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
  
  EIFR = 0xFF; // clear all pending external interrupt
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_SYS), sys_start, FALLING);
  Statistic_Init();
  
  Serial.println("Program starts!");
}

void loop() {
  if ((ucBootResultFlags & BOOT_RESULT_FLAG_RF_ERROR) == 0) {
    while (radio.available()){
      radio.read(&signalState, sizeof(signalState));
      if (signalState == HIGH && sys_active==false) { 
        bRFReceived = true;
      }
    }
  }

  if ( (bSysButtonPressed) && (!bRFReceived) ) {
      if ((ucBootResultFlags & BOOT_RESULT_FLAG_RF_ERROR) == 0) {
          radio.stopListening();
          signalState = 1;
          radio.write(&signalState, sizeof(signalState));
          radio.startListening();
      }
  }

  if ( ((bSysButtonPressed) || (bRFReceived)) && (!should_wait) ) {
    bSysButtonPressed = false;
    
    if(sys_active==false) {

        sys_countdown = SYS_INTERVAL;
        stand_still_countdown = STAND_STILL_INTERVAL;
        Statistic_ButtonActivate();
        TCNT4=1950;//reset timer
        TCCR4B = (1<<WGM42) | (1<<CS40) | (1<<CS42); // Set the prescale 1/1024 clock
        TIMSK4 = (1<<OCIE4A); 
        start_go();
        sys_active = true;
        
    }
    bRFReceived = false;
  } else if (((bSysButtonPressed) || (bRFReceived)) && ( (should_wait) && (!wait_started) )) {
    TCNT3=15623;//reset timer
    start_wait();
    wait_started = true;
  }
  
  if (bEvent1Hz) {

    if(should_wait) {
      wait_countdown--;
      if(wait_countdown == 0)
        should_wait = false;
        stop_wait();
    }
    
    bEvent1Hz = false;
    Statistic_1HzHook();
  }
}
  
void sys_start(){
  bSysButtonPressed = true;
  if(should_wait) {
    start_wait();
  }
}

void sys_stop() {
  stop_go();
  TCCR4B = 0;
  TIMSK4 = 0;  
  
  sys_active = false;
  should_wait = true;
  wait_started = false;
  wait_countdown = WAIT_INTERVAL;
  signalState = 0;

  
  digitalWrite(PIN_RELAY_BLINKING, LOW);
  digitalWrite(PIN_RELAY_BLINKING2, LOW);
  digitalWrite(PIN_RELAY_PROJECTOR , LOW);
}

void start_wait() {
  digitalWrite(PIN_COUNTDOWN_RED, LOW);
  digitalWrite(24, HIGH);
}

void stop_wait() {
  digitalWrite(PIN_COUNTDOWN_RED, HIGH);
  digitalWrite(24, LOW);
}

void start_go() {
  digitalWrite(PIN_COUNTDOWN_GREEN, LOW);
  digitalWrite(25, HIGH);
}

void stop_go() {
  digitalWrite(PIN_COUNTDOWN_GREEN, HIGH);
  digitalWrite(25, LOW);
}
