/*
transmitter
Defeat nRF24L01 in three steps
*/

#include <SPI.h>
#include <RF24.h>
RF24 radio(48, 49); 
const uint32_t pipe = 111156789; 

byte data;

void setup() {
  Serial.begin(115200);
  Serial.println("TransmitterTester ON");

  radio.begin();               
  delay(2000);
  radio.setDataRate(RF24_1MBPS); 
  radio.setCRCLength(RF24_CRC_8); 
  radio.setChannel(0x6f);         
  radio.setAutoAck(false);       
  radio.powerUp();
  radio.setPALevel(RF24_PA_MAX);               
  radio.stopListening();  // do not listen to the radio, only transmission
  radio.openWritingPipe(pipe);   // open pipe for sending
}

void loop() {
  data = 109;
  radio.write(&data, 1);
  Serial.println("data= " + String(data));
}
