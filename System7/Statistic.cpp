#include "Statistic.h"
#include <Arduino.h>
#include <LiquidCrystal.h>
#include "DeviceConfig.h"
#include "Battery.h"

#define STATUS_SHOW_INTERVAL      10   // in sec

LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
unsigned char ucBootResultFlags = 0;
unsigned char StatusShowCountDown = 0;
unsigned long ulButtonActivateCount = 0;
unsigned long ulButtonActivateRecord24Hour = 0;
unsigned long ulButtonActivateRecordCountdown;

void Statistic_Show(){
  if (StatusShowCountDown == 0)
  {    
    // Turn on the LCD
    digitalWrite(PIN_LCD_VDD, HIGH);
    digitalWrite(PIN_LCD_BL, HIGH);
    lcd.begin(16, 2);
    lcd.print("B: ");
    lcd.print(ucBootResultFlags, HEX);
    lcd.print(" 1D: ");
    lcd.print(ulButtonActivateRecord24Hour);
    lcd.print("      ");
    Serial.print("Boot: 0x");
    Serial.println(ucBootResultFlags, HEX);
    Serial.print("Key 24Hr: ");
    Serial.println(ulButtonActivateRecord24Hour);
  }
  StatusShowCountDown = STATUS_SHOW_INTERVAL + 1;
}

void Statistic_Init()
{
  // Initialize the LCD
  pinMode(PIN_LCD_VDD,OUTPUT);
  pinMode(PIN_LCD_BL,OUTPUT);
  pinMode(PIN_LCD_V0,OUTPUT);
  pinMode(PIN_LCD_RW,OUTPUT);
  digitalWrite(PIN_LCD_VDD,LOW);
  digitalWrite(PIN_LCD_BL,LOW);
  digitalWrite(PIN_LCD_V0,LOW); 
  digitalWrite(PIN_LCD_RW,LOW);
  
  pinMode(PIN_BUTTON_STATUS, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_STATUS), Statistic_Show, FALLING);

  ulButtonActivateRecordCountdown = 24*60*60;
}

void Statistic_ButtonActivate()
{
  ulButtonActivateCount++;
}

void Statistic_1HzHook()
{
  unsigned int uiBatteryVoltage; 
  unsigned char ucBatteryPresentage = 0xFF; // invalid
  
  if (StatusShowCountDown)
  {
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
    { // Stop showing status
      // Turn off LCD
      digitalWrite(PIN_LCD_VDD, LOW);
      digitalWrite(PIN_LCD_BL, LOW);      
    }
  }
  
  ulButtonActivateRecordCountdown--;
  if (ulButtonActivateRecordCountdown == 0)
  { // 24 Hour expired
    ulButtonActivateRecordCountdown = 24*60*60;
    ulButtonActivateRecord24Hour = ulButtonActivateCount;
    ulButtonActivateCount = 0;
  }
}
