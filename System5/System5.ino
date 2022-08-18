#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <DFRobot_RGBMatrix.h> // Hardware-specific library
#include <Wire.h>
#define OE    9
#define LAT   10
#define CLK   11
#define A     A0
#define B     A1
#define C     A2
#define D     A3
#define E     A4
#define WIDTH 64
#define HIGH  64
int a=0;
int b=16;
DFRobot_RGBMatrix matrix(A, B, C, D, E, CLK, LAT, OE, false, WIDTH, HIGH);

int number = 5;

int button = 2;
int relay1 = 5;
int relay2 = 6;

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
const unsigned char* address2 = addresses[0];;
const unsigned char* address1 = addresses[1];;
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
    timer++;

    if(timer%1000==0){;
  
      matrix.fillScreen(matrix.Color333(0, 0, 0));
  
      matrix.setTextSize(4); 
      matrix.setCursor(10, 25); 
      matrix.setTextColor(matrix.Color333(4, 0, 4));//
      matrix.println(number);
  
      number--;
    }

    if (timer%5000==0){
      sys_stop();
    }

    if(timer%300==0){
      static boolean output = HIGH;
      digitalWrite(relay1, output);
      output = !output;
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

  matrix.begin();
  matrix.fillScreen(0);//clear the screen

}

void loop() {
  delay(300);  
    //receiving signal and reading it!
//    radio.startListening();
//    while (radio.available()){
//      radio.read(&signalState, sizeof(signalState));
//      if (signalState == HIGH && sys_active==false) { 
//        sys_start();
//      }
//    }
  
  
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
  matrix.fillScreen(matrix.Color333(0, 0, 0));
}
