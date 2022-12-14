/*
receiver
Defeat nRF24L01 in three steps
*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(48, 49); 

const uint32_t pipe = 111156789; 
byte data[1];
int scn;  // counter of broadcast listening cycles
int sg;  // counter of the number of received packets from the transmitter

void setup() {
  Serial.begin(115200);
  Serial.println("ReceiverTester ON");

  radio.begin();  
  delay(2000);
  radio.setDataRate(RF24_1MBPS); 
  radio.setCRCLength(RF24_CRC_8); 
  radio.setChannel(0x6f);         // set the channel
  radio.setAutoAck(false);       
  radio.openReadingPipe(1, pipe); // open the pipe for receiving
  radio.startListening();        // receive
}

void loop() {
  if (scn < 200)
  { // listening to the air
    if (radio.available())
    {
      radio.read(data, 1);

      if (data[0] == 109) {
        sg++;
      }
    }
  } else {//total received
    {
      Serial.println("Received " + String(sg) + " packages");
      sg = 0;
    }
    scn = 0;
  }
  scn++;
  delay(5);

  if (scn >= 200) scn = 200; //counter overflow protection
}
