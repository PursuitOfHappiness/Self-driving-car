/*
 * Sketch for steering manually and with RC-Controller
 * Created for the autonomous car project at Gothenburg University
 */
#include <Servo.h>      // Steering and motor

const int rcPinSteer = 3; // rc steering
const int rcPinESC = 9;  // rc motor

const int servoPin = 5;   // pin to which the servo motor is attached
const int escPin = 6;     // pin to which the ESC is attached

Servo motor, steering;
unsigned long rcControllerFlag;
int controlFlag;

void setup() {
  //SERIAL CONNECTION
  Serial.begin(57600);

  motor.attach(escPin);
  motor.writeMicroseconds(1500);  // set motor to neutral
  steering.attach(servoPin);
  steering.write(90);  // set servo to neutral
  attachInterrupt(digitalPinToInterrupt(3), rcControllerInterrupt, RISING); // interupts from rc controller

  rcControllerFlag = 0; // Set to 1 if RC controller is turned on (interupt)
  controlFlag = 1; // Set to 0 when the RC takes over, used to set steering and speed to neutral when RC controller is turned off

  while(!Serial); // Wait until Serial is ready

  Serial.println("Test sketch for rc-controller and manually setting speed/steering");
  Serial.println("'t' + 0-180 for steering. 'v' + 1000-2000 for speed");
}

void loop() {
  if (rcControllerFlag == 1) { // if an interupt is read from the RC-Controller
    rcControl();
    controlFlag = 0;
  } else if (controlFlag == 0) { // this is true only after the RC-Controller is turned off
    motor.writeMicroseconds(1500);
    steering.write(90);
    controlFlag = 1;
  } else { // If a full command has been read form the serial communication
    manualControl();
  }
}
/*
 * Function for manual control with an RC-Controller
 */
void rcControl() {
  int steer, velocity;
  velocity = pulseIn(rcPinESC, HIGH, 25000); // get a value from the RC-Controller
  velocity = constrain(velocity, 1100, 1900); // we dont want any values outside this range
  velocity = map(velocity, 1100, 1900, -100, 100); // map values for easier use

  steer = pulseIn(rcPinSteer, HIGH, 25000);
  steer = constrain(steer, 1090, 1750); // we dont want any values outside this range
  steer = map(steer, 1090, 1750, 60, 130); // map values for easier use

  if (velocity >= -5 && velocity <= 40) {
    motor.writeMicroseconds(1500);
  } else if (velocity > 40) {
    motor.writeMicroseconds(1575 + (velocity-25));
  } else if (velocity < -5) {
    motor.writeMicroseconds(1230 + velocity);
  }
  if (steer >= 90 && steer <= 95) {
    steering.write(90);
  } else if (steer > 95 ){
    steering.write(steer);
  } else if (steer < 90){
    steering.write(steer);
  }
  if (pulseIn(rcPinSteer, HIGH, 25000) == 0) {
    rcControllerFlag = 0;
  }
}
/*
 * Reads values from the serial port. Reads the int values and sets the steering
 * and motor to what it read. Use 't' for steering anv 'v' for motor
 */
void manualControl() {
  String input;
  int steer, velocity;
  if (Serial.available() > 0) {
    input = Serial.readStringUntil('\n');

    if (input.startsWith("t")) { // turning
      steer = input.substring(1).toInt();
      if (steer <= 180 && steer >= 0) { // check that the value is in range
        steering.write(steer);
        Serial.println("Turning set to: ");
        Serial.println(steer);
      }
    } else if (input.startsWith("v")) { // velocity
      velocity = input.substring(1).toInt();
      if (velocity <= 2000 && velocity >= 1000) { // check that the value is in range
        motor.writeMicroseconds(velocity);
        Serial.print("Velocity set to: ");
        Serial.println(velocity);
      }
    }
  }
}
/*
 * Listens for the interupts from the RC-Controller
 */
void rcControllerInterrupt() {
  rcControllerFlag = 1;
}
