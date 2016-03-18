// --------- //
// Libraries //
// --------- //
#include <Servo.h> 

// --------- //
// Constants //
// --------- //
const int servoPin = 9; // pin to which the servo motor is attached
const int escPin = 6; // pin to which the ESC is attached
const int rcPinSteer = 3; // rc steering
const int rcPinESC = 5;  // rc motor

// ----------------------- //
// Instatiation of objects //
// ----------------------- //
Servo motor, steering;
unsigned long rcControllerFlag;
String input;
int steer, velocity;

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
  rcControllerFlag = pulseIn(rcPinSteer, HIGH, 25000); // if the timeout is lower it sometimes time out before getting a value
  //Serial.print("Pulse read: ");
  //Serial.println(rcControllerFlag);
  if(rcControllerFlag > 0){
    //motor.writeMicroseconds(1500);
    //steering.write(90);
    rcControl();
  }else{

    manualControl();
    //handleInput();
  }
}

void rcControl(){
  steer = map(pulseIn(rcPinSteer, HIGH, 25000), 1000, 2000, 0, 180);
  velocity = pulseIn(rcPinESC, HIGH, 25000);
  int i;
  int sum = 0;
  for(i = 0; i < 10; i++){
    sum += steer;
  }
  sum = sum / 10 + 7;
  Serial.print("steer ");
  Serial.println(floor(sum));
  Serial.print("velocity ");
  Serial.println(velocity);
  steering.write(floor(sum));
  motor.writeMicroseconds(3000 - velocity);
}

void manualControl(){
  if (Serial.available()){
    input = Serial.readStringUntil('\n');
    
    if (input.startsWith("s")){  // steering
      steer = input.substring(1).toInt();
      if (steer <= 180 && steer >=0){  // check that the value is in range
        steering.write(steer);
        Serial.println("Steering set to: ");
        Serial.println(steer);
      }
    }else if (input.startsWith("v")){  // velocity
      velocity = input.substring(1).toInt();
      if (velocity <= 2000 && velocity >= 1000){  // check that the value is in range
        motor.writeMicroseconds(velocity);
        Serial.print("Velocity set to: ");
        Serial.println(velocity);
      }
    }
  }
}

void handleInput() { //handle serial input if there is any
  if (Serial.available()) {
    input = Serial.readStringUntil('\n');
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
    input = Serial.readStringUntil('\n');
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

