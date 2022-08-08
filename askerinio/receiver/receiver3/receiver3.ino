#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define led 12
#define button 13

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
boolean buttonState = 0;

unsigned long last = millis();
unsigned long mx_time = 10000;
unsigned long curr;
bool activ=false;
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
  
  if(curr==0){
    long long int tm=4320000000;
    last=tm-last;
    
  }
  curr=millis();
  if(activ==true && curr - last >= mx_time){
    last=curr;  
    Serial.println(curr);
    turnLedOff();

  }
  else if(curr - last >= mx_time){
    Serial.println(curr);
    last=curr;  
    
  }
  radio.stopListening();
  if(activ==false && buttonIsPressed()==true){
      turnLedOn();
      buttonState = digitalRead(button);
      radio.write(&buttonState, sizeof(buttonState));
      last=millis();
  }
    delay(5);
    radio.startListening();
    while (radio.available()){
      radio.read(&buttonState, sizeof(buttonState));
      if (buttonState == HIGH && activ==false) { 
          turnLedOn();
          last=millis();
      }
    }
}

bool buttonIsPressed() {
    return digitalRead(button) == HIGH;
}

void turnLedOff() {
    digitalWrite(led, LOW);
    activ=false;
}

void turnLedOn() {
    digitalWrite(led, HIGH);
    activ=true;
    //delay(3000);
}
