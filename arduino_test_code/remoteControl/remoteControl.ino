// --------- //
// Libraries //
// --------- //
#include <Servo.h> 

// --------- //
// Constants //
// --------- //
//MOTOR
const int servoPin = 9; //pin to which the servo motor is attached
const int escPin = 6; //pin to which the ESC is attached
const int rcPin = 3;

// ----------------------- //
// Instatiation of objects //
// ----------------------- //
Servo motor, steering;
unsigned long rcControllerFlag;

void setup() {
  //SERIAL CONNECTION
  Serial.begin(9600);

  motor.attach(escPin);
  motor.writeMicroseconds(1500);  // set servo to mid-point
  steering.attach(servoPin);
  steering.write(90);  // set servo to mid-point
  rcControllerFlag = 0;
}

void loop() {
  rcControllerFlag = pulseIn(rcPin, HIGH, 25000);
  Serial.print("Pulse read: ");
  Serial.println(rcControllerFlag);
  if(rcControllerFlag > 0){
   motor.writeMicroseconds(1500);
   steering.write(90);
  }
  handleInput();
}

void handleInput() { //handle serial input if there is any
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input.startsWith("w")) {
      motor.writeMicroseconds(1570);
      Serial.println("Going forward");
    }else if (input.startsWith("s")){
      motor.writeMicroseconds(1200);
      Serial.println("Going Backward");
    }else if (input.startsWith("q")){
      motor.writeMicroseconds(1500);
      Serial.println("Neutral");
    }else if (input.startsWith("a")){
      steering.write(60);
      Serial.println("Turning Left");
    }else if (input.startsWith("d")){
      steering.write(120);
      Serial.println("Turning Right");
    }else if (input.startsWith("x")){
      steering.write(90);
      Serial.println("Straight Forward");
    }
  }
}

void calibrateESC() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    if (input.startsWith("u")){
        motor.writeMicroseconds(2000);
        Serial.println("Full forward");
      }else if (input.startsWith("i")){
        motor.writeMicroseconds(1000);
        Serial.println("Full back");
      }else if (input.startsWith("n")){
        motor.writeMicroseconds(1500);
        Serial.println("Neutral");
      }
      
  }
}

