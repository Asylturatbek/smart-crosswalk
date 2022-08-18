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

int button = 2;
int relay1 = 5;
int relay2 = 6;

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};
const unsigned char* address2 = addresses[0];;
const unsigned char* address1 = addresses[1];;
boolean signalState = 0;

int sys_active = 0;
int counter_time=0;
int number = 5;
boolean output = HIGH;

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
  TCCR1A = 0;
  TCCR1B = 0; 
  TCNT1 = 0;//initialize counter value to 0
  OCR1A = 6250;
  TCCR1B = (1<<WGM12) | (1<<CS12); 
  TIMSK1 = (1<<OCIE1A); 
  sei(); 
  Serial.println("TIMER1 Setup Finished.");

  matrix.begin();
//  matrix.fillScreen(0);//clear the 

}

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
  if (sys_active) {
    if (counter_time % 5 == 0) {
      digitalWrite(relay1, output);
      output = !output;
    }

//    if (counter_time % 10 == 0) {
//      
//  
//      number--;
//    }
   
    counter_time++;

    if(counter_time%50==0){
      sys_stop();
    }
  }
  
} 

void loop() {
  delay(5);
  //in this way you can count 1 second because the nterrupt request is each 1ms

  //receiving signal and reading it!
  radio.startListening();
  while (radio.available()){
    radio.read(&signalState, sizeof(signalState));
    if (signalState == HIGH && sys_active==false) { 
      sys_start();
    }
  }

//  matrix.setTextSize(1); 
//      matrix.setCursor(5, 5);    // start at top left, with 8 pixel of spacing
//      //matrix.setTextColor(matrix.Color333(14, 2, 25));//
//      matrix.println("The Tech!");
//  
//      matrix.setTextSize(4); 
//      matrix.setCursor(10, 25); 
//      matrix.setTextColor(matrix.Color333(4, 0, 4));//
//      matrix.println(number);

}
  
void sys_start(){

  if (!sys_active){
  
    radio.stopListening();//sending the signal to other module!
    if(sys_active==false){
        signalState = 1;
        radio.write(&signalState, sizeof(signalState));
    }
    
    //delay(40);    // adding a small delay prevents reading the buttonState to fast
    sys_active = true;
    digitalWrite(relay2, HIGH);
  }
  
}

void sys_stop(){
  sys_active = false;
  signalState = 0;
  digitalWrite(relay2, LOW);
  digitalWrite(relay1 , LOW);
  number = 5;
}
