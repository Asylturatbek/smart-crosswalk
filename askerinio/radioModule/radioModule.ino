#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#define relay 11
#define ledo 12
#define button 13

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
const byte address1 = addresses[0];
const byte address2 = addresses[1];
boolean buttonState = 0;

unsigned long last_start = 0;
unsigned long mx_time = 5000;
bool active=false;
void setup() {
  Serial.begin(9600);
  pinMode(button, INPUT);
  pinMode(ledo, OUTPUT);
  pinMode(relay, OUTPUT);
  turnLedOff();
  radio.begin();
  radio.openWritingPipe(address1); // 00002
  radio.openReadingPipe(1, address2); // 00001
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  delay(5);

  if(active==true && millis() - last_start >= mx_time){
    turnRelayOff();
    turnLedOff();
    Serial.println("Turned off");
  }

  radio.stopListening();
  if(active==false && buttonIsPressed()==true){
      buttonState = digitalRead(button);
      radio.write(&buttonState, sizeof(buttonState));
      turnRelayOn();
      turnLedOn();
      last_start=millis();  
  }

  
  delay(5);
  radio.startListening();
  while (radio.available()){
    radio.read(&buttonState, sizeof(buttonState));
    if (buttonState == HIGH && active==false) { 
        turnRelayOn();
        turnLedOn();
        last_start=millis();
    }
  }
  
  
}

bool buttonIsPressed() {
    return digitalRead(button) == HIGH;
}
void turnRelayOff(){
    digitalWrite(relay,HIGH);
    active=false;
    }
void turnRelayOn(){
    digitalWrite(relay,LOW);
    active=true;
    }
void turnLedOff() {
    digitalWrite(ledo, LOW);
    active=false;
}

void turnLedOn() {
    digitalWrite(ledo, HIGH);
    active=true;
}
