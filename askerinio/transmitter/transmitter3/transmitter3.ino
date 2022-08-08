#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define ledo 12
#define button 13

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
boolean buttonState = 0;

void setup() {
  pinMode(button, INPUT);
  pinMode(ledo, OUTPUT);
  turnLedOff();
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[1]); // 00002
  radio.openReadingPipe(1, addresses[0]); // 00001
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  delay(5);

  radio.stopListening();

  if (buttonIsPressed()) {
    turnLedOn();
  }
  else {
    turnLedOff();
  }
 
  buttonState = digitalRead(button);
  radio.write(&buttonState, sizeof(buttonState));

  delay(5);
  radio.startListening();
  while (radio.available()){
    radio.read(&buttonState, sizeof(buttonState));
    if (buttonState == HIGH || buttonIsPressed()) { 
        turnLedOn();
    }
    else {
      turnLedOff();
    }
  }
  
}

bool buttonIsPressed() {
    return digitalRead(button) == HIGH;
}

void turnLedOff() {
    digitalWrite(ledo, LOW);
}

void turnLedOn() {
    digitalWrite(ledo, HIGH);
    //delay(3000);
}
