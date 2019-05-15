
// Example Sketch for Keyes K853518 LDR Module (Light Dependent PhotoResistor).
// It reads the voltage at the OUT Pin and displays it on the console. The value should
// range from 0 to 1024. The lower the value the lower the light. The higher 
// the value means more light being received by the LDR.
// Written by cactus.io. For more details and how to hookup the Keyes K853518 LDR module to
// the Arduino then visit this link.
// http://cactus.io/hookups/sensors/light/ldr/hookup-arduino-to-keyes-k853518-light-sensor-module

int OUT_Pin = A0;
 
void setup() {
    Serial.begin(9600);
    Serial.println("Keyes K853518 LDR Module Example Sketch | cactus.io");
}
 
void loop() {
   Serial.println(analogRead(OUT_Pin));
   delay(500);
}
