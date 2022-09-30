int led1 = 4;
int led2 = 5;
int led3 = 6;
int led4 = 7;
int button = 2;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
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

  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  digitalWrite(led3, HIGH);
  digitalWrite(led4, HIGH);

}

void loop() {
  

}

void sys_start(){
  
  sys_active = true;
  
  
}

void start_sound(){
   digitalWrite(led1, HIGH);
}

void start_display(){
   digitalWrite(led2, HIGH);
}

void start_gobo(){
  digitalWrite(led3, HIGH);
}
