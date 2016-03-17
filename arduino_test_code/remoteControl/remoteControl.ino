// --------- //
// Libraries //
// --------- //
#include <Servo.h> 

// --------- //
// Constants //
// --------- //
//MOTOR
const int servoPin = 5; //pin to which the servo motor is attached
const int escPin = 0; //pin to which the ESC is attached


// ----------------------- //
// Instatiation of objects //
// ----------------------- //
Servo motor, steering;
  
void setup() {
  //SERIAL CONNECTION
  Serial.begin(9600);

  motor.attach(escPin);
  motor.write(90);  // set servo to mid-point
  steering.attach(servoPin);
  steering.write(90);  // set servo to mid-point
}

void loop() {
  motor.write(120); // set motor to go forward at 1/3 speed
  //motor.write(60);  // set motor to go backwards at 1/3 speed
  //motor.write(90);  // stop motor
  //steering.write(120); // steer right? 1/3 of max
  //steering.write(60); // steer left? 1/3 of max
  //steering.write(90); // straight
  //handleInput();
  
}

void handleInput() { //handle serial input if there is any
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input.startsWith("w")) {
      motor.write(120);
      Serial.println("Going forward");
    }else if (input.startsWith("s")){
      motor.write(60);
      Serial.println("Going Backwards");
    }else if (input.startsWith("q")){
      motor.write(90);
      Serial.println("Stopping");
    }else if (input.startsWith("a")){
      steering.write(120);
      Serial.println("Turning Left");
    }else if (input.startsWith("d")){
      steering.write(120);
      Serial.println("Turning Right");
    }
  }
}

