#include <LiquidCrystal.h>
#include "Battery.h"

#define OUTPUT_STATUS_PERIOD  1   // in sec
#define BATTERY_DETECT_PIN    A0

unsigned char ucOutputStatusCount = OUTPUT_STATUS_PERIOD;
boolean b1HzEvent = false;
const int rs = 48, en = 44, d4 = 34, d5 = 32, d6 = 30, d7 = 28;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

ISR(TIMER1_COMPA_vect){  //This is the interrupt request
    b1HzEvent = true;
}

void setup() {
  cli();
  Serial.begin(9600);
  Serial.print("Initialising...");
    
  //attachInterrupt(0, sys_start, FALLING);
 
  // Setting up the LCD
  pinMode(26,OUTPUT);
  pinMode(24,OUTPUT);
  pinMode(52,OUTPUT);
  pinMode(50,OUTPUT);
  pinMode(46,OUTPUT);
  digitalWrite(24,LOW); 
  digitalWrite(26,HIGH); // Backlight 
  digitalWrite(46,LOW); 
  digitalWrite(50,LOW);
  digitalWrite(52,HIGH); // VDD

  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Status:");

  // Setting up timer1 for 1Hz interrupt
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 15624;
  TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12); // Set the prescale 1/1024 clock
  TIMSK1 = (1<<OCIE1A);  
  sei();
  Serial.println("OK");
}

void loop() {
  unsigned int uiBatteryVoltage; 
  unsigned char ucBatteryPresentage = 0xFF; // invalid   
  if (b1HzEvent)
  { // 1Hz Handler
    b1HzEvent = false;
    ucOutputStatusCount--;
    if (ucOutputStatusCount == 0)
    {
      ucOutputStatusCount = OUTPUT_STATUS_PERIOD;
      uiBatteryVoltage = Battery_GetVoltage(analogRead(BATTERY_DETECT_PIN));      
      ucBatteryPresentage = Battery_GetPresentage(uiBatteryVoltage);
      lcd.setCursor(0, 1);
      lcd.print((int)ucBatteryPresentage);
      lcd.print("% (");
      lcd.print(uiBatteryVoltage/100);
      lcd.print(".");
      lcd.print(uiBatteryVoltage%100);
      lcd.print(")V   ");

      Serial.print("Status: ");
      Serial.print((int)ucBatteryPresentage);
      Serial.print("% (");
      Serial.print(uiBatteryVoltage/100);
      Serial.print(".");
      Serial.print(uiBatteryVoltage%100);
      Serial.println(")V");
    }
  }
}
