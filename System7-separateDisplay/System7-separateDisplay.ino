#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DeviceConfig.h"
#include "Statistic.h"
#include "pitches.h"

#define SYS_INTERVAL              5   // in sec

RF24 radio(PIN_RF_CE, PIN_RF_CSN); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
const unsigned char* address2 = addresses[0];;
const unsigned char* address1 = addresses[1];;
boolean signalState = 0;
int sys_active = 0;
unsigned long timer=0;
unsigned long sys_countdown;
boolean bEvent1Hz = false;
boolean bSysButtonPressed = false;
boolean bRFReceived = false;
unsigned char ButtonPressGuardCountDown = 0;
unsigned char RedCountDown = 0;
boolean should_wait = false;
int wait_countdown = 0;

ISR(TIMER4_COMPA_vect){  //This is the interrupt request
  if(sys_active) {
    if(timer == 0){
      if (sys_countdown == 0)
      {
        sys_stop();
        return;
      }
      else
      {
        tone(PIN_SPEAKER, NOTE_C5, 200);
        sys_countdown--; 
      }
    }
    if (timer == 2)
      tone(PIN_SPEAKER, NOTE_FS4, 200);
    //if ((timer == 0) || (timer == 2) || (timer == 4) || (timer == 6)){
    if ((timer & 0x01) == 0){
      static boolean output = HIGH;
      digitalWrite(PIN_RELAY_PROJECTOR, output);
      output = !output;
    }
    timer++;
    if (timer == 8)
      timer = 0;
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
  if ((ucBootResultFlags & BOOT_RESULT_FLAG_RF_ERROR) == 0)
  {
    while (radio.available()){
      radio.read(&signalState, sizeof(signalState));
      if (signalState == HIGH && sys_active==false) { 
        bRFReceived = true;
      }
    }
  }

  if ( ((bSysButtonPressed) || (bRFReceived)) && (!should_wait) )
  {
    bSysButtonPressed = false;
    
    if(sys_active==false){
        digitalWrite(PIN_RELAY_BLINKING, HIGH);
        if (bRFReceived == false)
        {
          //sending the signal to other module!
          if ((ucBootResultFlags & BOOT_RESULT_FLAG_RF_ERROR) == 0)
          {
            radio.stopListening();
            signalState = 1;
            radio.write(&signalState, sizeof(signalState));
            radio.startListening();
          }
        }

        sys_countdown = SYS_INTERVAL;
        Statistic_ButtonActivate();
        TCNT4=1950;//reset timer
        TCCR4B = (1<<WGM42) | (1<<CS40) | (1<<CS42); // Set the prescale 1/1024 clock
        TIMSK4 = (1<<OCIE4A);   
        start_go();
        digitalWrite(25, HIGH);
        sys_active = true;
        
    }
    bRFReceived = false;
  }
  
  if (bEvent1Hz) {

    if(should_wait){
      wait_countdown--;
      if(wait_countdown == 0)
        should_wait = false;
        stop_wait();
        digitalWrite(24, LOW);
    }
    
    bEvent1Hz = false;
    Statistic_1HzHook();
  }
}
  
void sys_start(){
  bSysButtonPressed = true;
  if(should_wait) {
    wait_countdown = SYS_INTERVAL;
    start_wait();
    digitalWrite(24, HIGH);
  }
}

void sys_stop(){
  stop_go();
  digitalWrite(25, LOW);
  TCCR4B = 0;
  TIMSK4 = 0;  
  
  sys_active = false;
  should_wait = true;
  wait_countdown = SYS_INTERVAL;
  signalState = 0;

  
  digitalWrite(PIN_RELAY_BLINKING, LOW);
  digitalWrite(PIN_RELAY_PROJECTOR , LOW);
}

void start_wait() {
  digitalWrite(PIN_COUNTDOWN_RED, LOW);
}

void stop_wait() {
  digitalWrite(PIN_COUNTDOWN_RED, HIGH);
}

void start_go() {
  digitalWrite(PIN_COUNTDOWN_GREEN, LOW);
}

void stop_go() {
  digitalWrite(PIN_COUNTDOWN_GREEN, HIGH);
}
