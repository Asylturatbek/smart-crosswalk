int sig = 7;

void setup() {
  pinMode(sig, OUTPUT);
  // put your setup code here, to run once:
  for (int i=0; i<7000; i++) {
    digitalWrite(sig, HIGH);
    delay(500);
    digitalWrite(sig, LOW);
    delay(2500);
  }

}

void loop() {
  // put your main code here, to run repeatedly:
  
}
