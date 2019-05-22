
#include <Servo.h>
Servo servo1, servo2, servo3, servo4;

#define forwardspeed 1700
#define turnspeed 1700
 
void setup() { 
  Serial.begin(9600);
  servo1.attach(D6);
  servo2.attach(D5);
  servo3.attach(D7);
  servo4.attach(D8);
  servo1.writeMicroseconds(1500);  // Stop 
  servo2.writeMicroseconds(1500);  // Stop 
  servo3.writeMicroseconds(1500);  // Stop 
  servo4.writeMicroseconds(1500);  // Stop 
} 

void forward(int speed){
  servo1.writeMicroseconds(speed);  
  servo2.writeMicroseconds(speed);  
  servo3.writeMicroseconds(speed);   
  servo4.writeMicroseconds(speed);   
}

void backward(int speed){
  
}

void moveright(){
  left(1700,1700);
}

void moveleft(){
  left(1300,1300);
}
void left(int speed1, int speed2){
  servo1.writeMicroseconds(speed1);  
  servo2.writeMicroseconds(speed1);  
  servo3.writeMicroseconds(speed2);   
  servo4.writeMicroseconds(speed2);   
}

void right(int speed1, int speed2){
  servo1.writeMicroseconds(speed1);  
  servo2.writeMicroseconds(speed1);  
  servo3.writeMicroseconds(speed2);   
  servo4.writeMicroseconds(speed2);   
}


void stop(){
  servo1.writeMicroseconds(1500);  // Stop 
  servo2.writeMicroseconds(1500);  // Stop 
  servo3.writeMicroseconds(1500);  // Stop 
  servo4.writeMicroseconds(1500);  // Stop 
}

void loop() { 
  /*Serial.println("Moving Clockwise");
  servo1.write(1700); //C-Clockwise - Antihorario
  servo2.write(1700); //C-Clockwise - Antihorario
  delay(1000);
    Serial.println("Stop");
  servo1.write(1500); //Stop
  delay(2000);*/
  //stop();

  moveleft();
} 
