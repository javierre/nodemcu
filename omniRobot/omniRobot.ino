#include <Servo.h>

// Declaramos las variables para controlar cada servo
Servo servoMotor1, servoMotor2, servoMotor3, servoMotor4;

void left(){
  servoMotor1.write(1000);
  servoMotor4.write(1000);
  servoMotor2.write(2000);
  servoMotor3.write(2000);
}

void right(){
  servoMotor1.write(2000);
  servoMotor4.write(1000);
  servoMotor2.write(2000);
  servoMotor3.write(1000);
}

void forward(){
  servoMotor1.write(2000);
  servoMotor4.write(1000);
  servoMotor2.write(2000);
  servoMotor3.write(1000);
}

void backward(){
  servoMotor4.write(2000);
  servoMotor1.write(1000);
  servoMotor3.write(2000);
  servoMotor2.write(1000);
}

void rotateright(){
  servoMotor1.write(2000);
  servoMotor4.write(2000);
  servoMotor2.write(2000);
  servoMotor3.write(2000);
}

void rotateleft(){
  servoMotor1.write(1000);
  servoMotor4.write(1000);
  servoMotor2.write(1000);
  servoMotor3.write(1000);
}

void setup() {
  // Iniciamos el monitor serie para mostrar el resultado
  Serial.begin(9600);

  servoMotor1.attach(D5);
  servoMotor2.attach(D6);
  servoMotor3.attach(D7);
  servoMotor4.attach(D8);

}

void loop() {

  //forward();
  //rotateright();
  //rotateleft();
  //backward();
  left();
}

