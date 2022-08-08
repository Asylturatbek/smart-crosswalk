const int movpin=2;
const int led=3;

void setup(){
  Serial.begin(9600);
  pinMode(movpin,INPUT);
  pinMode(led,OUTPUT);
}
void loop(){
  int val=digitalRead(movpin);
  if(val)
    digitalWrite(led,HIGH);
  else
  
    digitalWrite(led,LOW);
}
