int led1 = 4;
int led2 = 5;
int led3 = 6;
int led4 = 7;
int button = 2;

bool sys_active = 0;
static boolean output = HIGH;

void setup() {
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(button,INPUT);

  attachInterrupt(0, sys_start, FALLING);

}

void loop() {
  

}

void sys_start(){
  
//  sys_active = true;
  output = !output;
  
  digitalWrite(led1, output);
  digitalWrite(led2, output);
  digitalWrite(led3, output);
  digitalWrite(led4, output);
  
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
