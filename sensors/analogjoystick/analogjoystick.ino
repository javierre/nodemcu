

#define X_axis A0
#define ButtonP D6

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(ButtonP, INPUT_PULLUP);
  pinMode(X_axis, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int value=analogRead(X_axis);
  Serial.println(value);
  
  if(digitalRead(ButtonP)==LOW) Serial.println("Button Pressed!");

}
