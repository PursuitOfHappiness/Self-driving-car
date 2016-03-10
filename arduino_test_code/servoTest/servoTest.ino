#include <Servo.h>

uint8_t servoPin = 3;

Servo myServo;  
  
void setup() {
  Serial.begin(9600);
    
}

void loop() {

myServo.attach(servoPin);
delay(50);
  myServo.write(45);
  delay(50);
 myServo.write(135);
}
