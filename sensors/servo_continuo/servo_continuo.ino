
#include <Servo.h>
Servo servo1;
 
void setup() { 
  servo1.attach(D6);
  servo1.writeMicroseconds(1500);  // Stop 
} 

void loop() { 
  servo1.write(1700); //C-Clockwise - Antihorario
  delay(1000);
  servo1.write(1500); //Stop
  delay(2000);
} 
