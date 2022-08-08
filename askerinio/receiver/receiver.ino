#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 53); // CE, CSN
const byte addresses [][6] = {"00001", "00002"};
boolean button_state = 0;
int led_pin = 3;

void setup() {
pinMode(3, OUTPUT);
Serial.begin(9600);
radio.begin();
radio.openWritingPipe(addresses[0]);      //Setting the address at which we will send the data
radio.openReadingPipe(1, addresses[1]);  //Setting the address at which we will receive the data
radio.setPALevel(RF24_PA_MIN);       //You can set this as minimum or maximum depending on the distance between the transmitter and receiver.
radio.startListening();              //This sets the module as receiver
}

void loop()
{
if (radio.available())              //Looking for the data.
{
  delay(5);
  radio.startListening();
  char text1[32] = "";         
  //radio.read(&button_state, sizeof(button_state));    //Reading the data
  radio.write(&text1, sizeof(text1));
  Serial.println(text1);
  //Serial.println(button_state);

  
//  if(button_state == HIGH)
//  {
//  digitalWrite(led_pin,HIGH);
//  Serial.println(text);
//  Serial.println("going for now");
//  }
//  else
//  {
//  digitalWrite(led_pin,LOW);
//  //Serial.println(text);
//  }
}
  delay(5);
  radio.stopListening();                             //This sets the module as transmitter
  //button_stateA = digitalRead(button_pinA);
  //radio.write(&button_stateA, sizeof(button_stateA));
  const char text2[] = "Sending from receiver";
  radio.write(&text2, sizeof(text2));
}
