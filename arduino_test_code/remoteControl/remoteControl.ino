// --------- //
// Libraries //
// --------- //
#include <Smartcar.h> //Including Dimitri's library

// --------- //
// Constants //
// --------- //
//MOTOR
const int servoPin = 5; //pin to which the servo motor is attached
const int escPin = 0; //pin to which the ESC is attached


// ----------------------- //
// Instatiation of objects //
// ----------------------- //
Car porsche(useServo(servoPin), useESC(escPin)); //instantiation of the car

  int servo;
  int esc;
  
void setup() {
  // put your setup code here, to run once:

  porsche.begin();

  //SERIAL CONNECTION
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  handleInput();
  //porsche.setSpeed(80);
  //analogWrite(0);
}

void handleInput() { //handle serial input if there is any
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input.startsWith("w")) {
      porsche.go(5);
      Serial.println("Going forward");
    }else if (input.startsWith("s")){
      porsche.stop();
      Serial.println("Stopping");
    }else if (input.startsWith("a")){
      porsche.rotate(-90);
      Serial.println("Turning Left");
    }else if (input.startsWith("d")){
      porsche.rotate(90);
      Serial.println("Turning Right");
    }
  }
}
