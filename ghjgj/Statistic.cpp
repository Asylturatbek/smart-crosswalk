#include "Statistic.h"
#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "DeviceConfig.h"
#include "Battery.h"

#define STATUS_SHOW_INTERVAL      10   // in sec

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned char ucBootResultFlags = 0;
unsigned char StatusShowCountDown = 0;
unsigned long ulButtonActivateCount = 0;
unsigned long ulButtonActivateRecord24Hour = 0;
unsigned long ulButtonActivateRecordCountdown;

void Statistic_Show(){
  if (StatusShowCountDown == 0)
  {    
    Serial.print("Boot: 0x");
    Serial.println(ucBootResultFlags, HEX);
    Serial.print("Key 24Hr: ");
    Serial.println(ulButtonActivateRecord24Hour);
  }
  StatusShowCountDown = STATUS_SHOW_INTERVAL + 1;
}

void Statistic_Init()
{

    lcd.begin();
    lcd.noBacklight(); 
    
  // Initialize the LCD
  
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

    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("B: ");
    lcd.print(ucBootResultFlags, HEX);
    lcd.print(" 1D: ");
    lcd.print(ulButtonActivateRecord24Hour);
    lcd.print("      ");
    
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
      lcd.clear();
      lcd.noBacklight();    
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
