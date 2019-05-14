/* Interface Accelerometer with NodeMCU
 * By TheCircuit
 */

#include <math.h>

const int zPin = A0;    //z-axis of the Accelerometer 
const float Pi = 3.14159;


void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

}

void loop() {
  int sensorValue=analogRead(zPin);
  float zgravity=(float)(sensorValue-483)*9.8/100;
  delay(100);

  Serial.print("z angle : ");
  double angle = 180*acos(double(zgravity/9.8))/Pi;
  if(isnan(angle))angle=0;
  Serial.println(angle);
}
