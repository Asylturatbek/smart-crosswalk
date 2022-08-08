#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

int button = 2;
int led1 = 10;
int led2 = 11;

RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"00001", "00002"};

unsigned long mx_time = 5000;

unsigned int reload = 0x2904; 

int sys_active = 0;
int timer=0;

ISR(TIMER0_COMPA_vect){    //This is the interrupt request
  if(sys_active)
    timer++;
}

ISR(TIMER1_COMPA_vect)
{
  static boolean output = HIGH;
  digitalWrite(led1, output);
  output = !output;
}


void setup() {
  Serial.begin(9600);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(button,INPUT);

  attachInterrupt(0, sys_start, RISING);

  cli();
  TCCR0A=(1<<WGM01);    //Set the CTC mode   
  OCR0A=0xF9; //Value for ORC0A for 1ms 
  TIMSK0|=(1<<OCIE0A);   //Set the interrupt request
  TCCR0B|=(1<<CS01);    //Set the prescale 1/64 clock
  TCCR0B|=(1<<CS00);
  sei();
  Serial.println("Timer0 Setup finished");

  cli();
  TCCR1A = 0;
  TCCR1B = 0; 
  TCNT1 = 0;//initialize counter value to 0
  OCR1A = reload;
  TCCR1B = (1<<WGM12) | (1<<CS12); 
  //TIMSK1 = (1<<OCIE1A); 
  TIMSK1 &= ~(1 << OCIE1A);
  sei(); 
  Serial.println("TIMER1 Setup Finished.");

}

void loop() {
  delay(5);

  //in this way you can count 1 second because the nterrupt request is each 1ms
  if(sys_active==true && timer>=mx_time){
    sys_stop();
    timer=0;

    //stop the first timer that makes blink
    cli();
    TIMSK1 &= ~(1 << OCIE1A);
    sei();
  }
  
}
  
void sys_start(){
  //delay(40);             // adding a small delay prevents reading the buttonState to fast
  sys_active = true;
  digitalWrite(led2, HIGH);

  // enable timer compare interrupt
  cli();
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

void sys_stop(){
  sys_active = false;
  digitalWrite(led2, LOW);
  digitalWrite(led1 , LOW);
}
