#define ButtonP D8

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ButtonP, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(ButtonP)==LOW) Serial.println("Button Pressed!");
}
