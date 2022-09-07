#include <SD.h>                           //include SD module library
#include <TMRpcm.h>                       //include speaker control library

#define SD_ChipSelectPin 10              //define CS pin

TMRpcm tmrpcm;

const int LED_pin = 7; 
volatile byte count;
byte reload = 0xF9; 

ISR(TIMER2_COMPA_vect)
{
count++;
OCR2A = reload;
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_pin, OUTPUT);
  digitalWrite(LED_pin, LOW);
  cli();
  TCCR0B = 0; 
  OCR2A = reload;
  TCCR2A = 1<<WGM21;
  TCCR2B = (1<<CS22);
  TIMSK2 = (1<<OCIE2A);
  sei();
  Serial.println("TIMER2 Setup Finished.");

//  tmrpcm.speakerPin = 9; 
//  tmrpcm.quality(1);//define speaker pin.                                      
//  if (!SD.begin(SD_ChipSelectPin)) {      //see if the card is present and can be initialized
//    
//    return;                               //don't do anything more if not
//  }
//  
//  tmrpcm.setVolume(6);                    //0 to 7. Set volume level
//  tmrpcm.play("3.wav"); 
}
void loop()
{
if (count == 50)
{
flash();
Serial.print(".");
count = 0;
}
}
void flash()
{
static boolean output = HIGH;
digitalWrite(LED_pin, output);
output = !output;
}
