#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <DMD2.h>
#include <fonts/Arial_Black_16.h>

SoftDMD dmd(1,1);
// Number of P10 panels used X, Y
DMD_TextBox box(dmd, 7, 1, 32, 16); 
// Set Box (dmd, x, y, Height, Width) 

int number = 5;

int button = 2;
int relay1 = 5;
int relay2 = 4;

RF24 radio(48, 49); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
const unsigned char* address1 = addresses[0];;
const unsigned char* address2 = addresses[1];;
boolean signalState = 0;

int sys_active = 0;
unsigned long timer=0;

ISR(TIMER0_COMPA_vect){  //This is the interrupt request
  
  radio.startListening();
  while (radio.available()){
    radio.read(&signalState, sizeof(signalState));
    if (signalState == HIGH && sys_active==false) { 
      sys_start();
    }
  }
  
  if(sys_active) {

    if(timer%1000==0){
      box.clear();
      box.print(number);
      
      number--; 

    }

    if(timer%300==0){
      static boolean output = HIGH;
      digitalWrite(relay1, output);
      output = !output;
    }

    timer++;

    if (timer%5000==0){
      sys_stop();
    }
  }
  
}


void setup() {
  Serial.begin(9600);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(button,INPUT);

  radio.begin();
  radio.openWritingPipe(address1); // 00002
  radio.openReadingPipe(1, address2); // 00001
  radio.setPALevel(RF24_PA_MIN);

  attachInterrupt(0, sys_start, RISING);

  cli();
  TCCR0A=(1<<WGM01);    //Set the CTC mode   
  OCR0A=0xF9; //Value for ORC0A for 1ms 
  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
  TCCR0B|=(1<<CS01);    //Set the prescale 1/64 clock
  TCCR0B|=(1<<CS00);
  sei();
  Serial.println("Timer0 Setup finished");

  dmd.setBrightness(10); // Set brightness 0 - 255 
  dmd.selectFont(Arial_Black_16); // Font used
  dmd.begin();     // Start DMD 
  box.clear();


}

void loop() {

}
  
void sys_start(){

  //sending the signal to other module!
  radio.stopListening();
  if(sys_active==false){
      signalState = 1;
      radio.write(&signalState, sizeof(signalState));
  }
  
  //delay(40);             // adding a small delay prevents reading the buttonState to fast
  sys_active = true;
  digitalWrite(relay2, HIGH);

}

void sys_stop(){
  sys_active = false;
  signalState = 0;
  number = 5;
  digitalWrite(relay2, LOW);
  digitalWrite(relay1 , LOW);
  box.clear();
}
