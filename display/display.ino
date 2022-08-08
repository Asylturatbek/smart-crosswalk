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

int timer = 0;
int number = 25;

ISR(TIMER0_COMPA_vect){    //This is the interrupt request
  timer++;
}


void setup()
{

  cli();
  TCCR0A=(1<<WGM01);    //Set the CTC mode   
  OCR0A=0xF9; //Value for ORC0A for 1ms 
  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
  TCCR0B|=(1<<CS01);    //Set the prescale 1/64 clock
  TCCR0B|=(1<<CS00);
  sei();
  Serial.println("Timer0 Setup finished");

  
  matrix.begin();
  // fill the screen with 'black'
  matrix.setTextSize(1);     // size 1 == 8 pixels high
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  delay(500);

  matrix.fillScreen(0);//clear the screen


  matrix.setCursor(5, 5);    // start at top left, with 8 pixel of spacing
  matrix.setTextColor(matrix.Color333(14, 2, 25));//
  matrix.println("The Tech!");
  
}

void loop()
{  
  
  if(timer>=1000){
    if(number<0) number = 25;

    matrix.fillScreen(matrix.Color333(0, 0, 0));

    matrix.setTextSize(1); 
    matrix.setCursor(5, 5);    // start at top left, with 8 pixel of spacing
    //matrix.setTextColor(matrix.Color333(14, 2, 25));//
    matrix.println("The Tech!");

    matrix.setTextSize(4); 
    matrix.setCursor(10, 25); 
    matrix.setTextColor(matrix.Color333(4, 0, 4));//
    matrix.println(number);

    number--;
    timer=0;
  }
  
}
