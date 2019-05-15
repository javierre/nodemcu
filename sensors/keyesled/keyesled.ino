

#define Pinled D0

void setup() {
  // put your setup code here, to run once:
  pinMode(Pinled, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(Pinled, LOW);
  delay(1000);
  digitalWrite(Pinled, HIGH);
  delay(1000);

  digitalWrite(Pinled, 0);
  delay(1000);
  analogWrite(Pinled, 100);
  delay(1000);  
}
