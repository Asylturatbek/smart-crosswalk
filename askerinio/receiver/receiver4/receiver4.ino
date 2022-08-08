#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define led 12
#define button 13

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
boolean buttonState = 0;

unsigned long last_start = 0;
unsigned long mx_time = 3000;
bool active=false;
void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(button, INPUT);
  radio.begin();
  radio.openWritingPipe(addresses[1]); // 00002
  radio.openReadingPipe(1, addresses[0]); // 00001
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  delay(5);
  
  if(active==true && millis() - last_start >= mx_time){
    turnLedOff();
    Serial.println("Turned off");
  }  

  radio.stopListening();
  if(active==false && buttonIsPressed()==true){
      buttonState = digitalRead(button);
      radio.write(&buttonState, sizeof(buttonState));
      turnLedOn();
      last_start=millis();  
  }

  
  delay(5);
  radio.startListening();
  while (radio.available()){
    radio.read(&buttonState, sizeof(buttonState));
    if (buttonState == HIGH && active==false) { 
        turnLedOn();
        last_start=millis();
    }
  }

  
}

bool buttonIsPressed() {
    return digitalRead(button) == HIGH;
}

void turnLedOff() {
    digitalWrite(led, LOW);
    active=false;
}

void turnLedOn() {
    digitalWrite(led, HIGH);
    active=true;
}
