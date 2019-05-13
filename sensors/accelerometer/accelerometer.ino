/* Interface Accelerometer with NodeMCU
 * By TheCircuit
 */

const int zPin = A0;    //z-axis of the Accelerometer 

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

}

void loop() {
  int sensorValue=analogRead(zPin);
  float zgravity=(float)(sensorValue-483)*9.8/100;
  delay(100);

  Serial.print("z-axis : ");
  Serial.println(zgravity);

}
