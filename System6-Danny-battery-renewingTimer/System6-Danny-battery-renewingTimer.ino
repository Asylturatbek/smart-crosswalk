#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <DMD2.h>
#include <fonts/Arial_Black_16.h>
#include <LiquidCrystal.h>
#include "Battery.h"

#define PIN_BUTTON_SYS              2
#define PIN_BUTTON_STATUS           18
#define PIN_BATTERY_DETECT          A0
#define PIN_LCD_VDD                 A4
#define PIN_LCD_RS                  A2
#define PIN_LCD_EN                  A8
#define PIN_LCD_D4                  A9
#define PIN_LCD_D5                  A10
#define PIN_LCD_D6                  A11
#define PIN_LCD_D7                  A12
#define PIN_LCD_BL                  A6
#define PIN_LCD_RW                  A7
#define PIN_LCD_V0                  A3

#define STATUS_SHOW_INTERVAL      10   // in sec
#define SYS_INTERVAL              5   // in sec

#define BOOT_RESULT_FLAG_RF_ERROR  0x01

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

SoftDMD dmd(1,1);
// Number of P10 panels used X, Y
DMD_TextBox box(dmd, 7, 1, 32, 16); 
// Set Box (dmd, x, y, Height, Width) 

int number = 5;
int relay1 = 5;
int relay2 = 4;

RF24 radio(48, 49); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
const unsigned char* address1 = addresses[0];;
const unsigned char* address2 = addresses[1];;
boolean signalState = 0;
int sys_active = 0;
unsigned long timer=0;
unsigned long sys_countdown;
unsigned char StatusShowCountDown;
boolean bStatusEventStart = false;
boolean bStatusEvent1Hz = false;
unsigned char ucBootResultFlags = 0;

ISR(TIMER1_COMPA_vect){  //This is the interrupt request
  if(sys_active) {

    if(timer == 0){
      box.clear();
      box.print(number);
      number--;
      sys_countdown--; 
    }
    //if ((timer == 0) || (timer == 2) || (timer == 4) || (timer == 6)){
    if ((timer & 0x01) == 0){
      static boolean output = HIGH;
      digitalWrite(relay1, output);
      output = !output;
    }
    timer++;
    if (timer == 8)
      timer = 0;
      
    if (sys_countdown == 0)
        sys_stop();
  }
}

ISR(TIMER3_COMPA_vect){  //This is the interrupt request
  bStatusEvent1Hz = true;
}

void setup() {
  Serial.begin(9600);
  Serial.println("\r\nInitialization...");

  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(PIN_BUTTON_SYS, INPUT);
  pinMode(PIN_BUTTON_STATUS, INPUT_PULLUP);

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
  
  // Initialize Timer1 for 8 Hz interrupt
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 1953;
  TIMSK1 &= ~(1 << OCIE1A);

  // Initialize Timer3 for 1 Hz interrupt
  TCCR3A = 0;
  TCCR3B = 0;
  OCR3A = 15625;
  TIMSK3 &= ~(1 << OCIE3A);
  Serial.println("Timer: OK");
  sei();
  
  dmd.setBrightness(10); // Set brightness 0 - 255 
  dmd.selectFont(Arial_Black_16); // Font used
  dmd.begin();     // Start DMD 
  box.clear();

  // Initialize the LCD
  pinMode(PIN_LCD_VDD,OUTPUT);
  pinMode(PIN_LCD_BL,OUTPUT);
  pinMode(PIN_LCD_V0,OUTPUT);
  pinMode(PIN_LCD_RW,OUTPUT);
  digitalWrite(PIN_LCD_VDD,LOW);
  digitalWrite(PIN_LCD_BL,LOW);
  digitalWrite(PIN_LCD_V0,LOW); 
  digitalWrite(PIN_LCD_RW,LOW);

  EIFR = 0xFF; // clear all pending external interrupt
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_SYS), sys_start, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_STATUS), status_start, FALLING);

  Serial.println("Program starts!");
}

void loop() {
  unsigned int uiBatteryVoltage; 
  unsigned char ucBatteryPresentage = 0xFF; // invalid

  if ((ucBootResultFlags & BOOT_RESULT_FLAG_RF_ERROR) == 0)
  {
    while (radio.available()){
      radio.read(&signalState, sizeof(signalState));
      if (signalState == HIGH && sys_active==false) { 
        sys_start();
      }
    }
  }
  if (bStatusEventStart)
  { // Start status
    bStatusEventStart = false;
    // Turn on the LCD
    digitalWrite(PIN_LCD_VDD, HIGH);
    digitalWrite(PIN_LCD_BL, HIGH);
    lcd.begin(16, 2);
    lcd.print("Boot: 0x");
    lcd.print(ucBootResultFlags, HEX);
    Serial.print("Boot Result: 0x");
    Serial.println(ucBootResultFlags, HEX);
  }
  if (bStatusEvent1Hz)
  {
    bStatusEvent1Hz = false;
    StatusShowCountDown--;
    
    // Measure battery
    uiBatteryVoltage = Battery_GetVoltage(analogRead(PIN_BATTERY_DETECT));      
    ucBatteryPresentage = Battery_GetPresentage(uiBatteryVoltage);
    
    lcd.setCursor(0, 1);
    lcd.print("Bat: ");
    lcd.print((int)ucBatteryPresentage);
    lcd.print("% ");
    lcd.print(uiBatteryVoltage/100);
    lcd.print(".");
    lcd.print(uiBatteryVoltage%100);
    lcd.print("V ");

    Serial.print("Bat: ");
    Serial.print((int)ucBatteryPresentage);
    Serial.print("% (");
    Serial.print(uiBatteryVoltage/100);
    Serial.print(".");
    Serial.print(uiBatteryVoltage%100);
    Serial.println("V)");
  
    if (StatusShowCountDown == 0)
    { // Stop status
      TCCR3B = 0;
      TIMSK3 &= ~(1<<OCIE3A);
      // Turn off LCD
      digitalWrite(PIN_LCD_VDD, LOW);
      digitalWrite(PIN_LCD_BL, LOW);      
    }
  }
}
  
void sys_start(){
  digitalWrite(relay2, HIGH);
  
  //sending the signal to other module!
  if(sys_active==false){
      if ((ucBootResultFlags & BOOT_RESULT_FLAG_RF_ERROR) == 0)
      {
        radio.stopListening();
        signalState = 1;
        radio.write(&signalState, sizeof(signalState));
        radio.startListening();
      }
      sys_countdown = SYS_INTERVAL;
  }

  TCNT1=1950;//reset timer
  TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); // Set the prescale 1/1024 clock
  TIMSK1 = (1<<OCIE1A);
  
  //delay(40);             // adding a small delay prevents reading the buttonState to fast
  sys_active = true;
  Serial.println("BUTTON");
  digitalWrite(relay2, HIGH);
}

void sys_stop(){
  cli();
  TCCR1B = 0;
  TIMSK1 = 0;  
  sei();
  
  sys_active = false;
  signalState = 0;
  number = 5;
  digitalWrite(relay2, LOW);
  digitalWrite(relay1 , LOW);
  box.clear();
}

void status_start(){
  TCCR3B = (1<<WGM32) | (1<<CS30) | (1<<CS32); // Set the prescale 1/1024 clock
  TIMSK3 = (1<<OCIE3A);
  TCNT3=10000; //reset timer
  StatusShowCountDown = STATUS_SHOW_INTERVAL + 1;
  bStatusEventStart = true;
  Serial.println("STATUS");
}
