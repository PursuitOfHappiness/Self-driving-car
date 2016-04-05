// --------- //
// Libraries //
// --------- //
#include <Servo.h> 
#include <Smartcar.h> //Including Dimitri's library for sensors
#include <Netstrings.h> //Include Dimitri's library for netstrings

// --------- //
// Constants //
// --------- //
//SERVO
const int rcPinSteer = 3; // rc steering
const int rcPinESC = 22;  // rc motor

//MOTOR -- Digital
const int servoPin = 5; //pin to which the servo motor is attached
const int escPin = 6; //pin to which the ESC is attached

//SONARS  -- I2C
unsigned short sonarFrontAddress = 0x73; //pin to which the front sonar sensor is attached 
unsigned short sonarRightAddress = 0x70; //pin to which the front right sonar sensor is attached

//INFRAREDS  -- Analog
const int irFrontRightPin = 0;     //pin to which the front right infrared sensor is attached 
const int irRearRightPin = 1;      //pin to which the rear right infrared sensor is attached 
const int irRearCenterPin  = 2;    //pin to which the rear infrared sensor is attached 

//WHEEL ENCODERS
const int pulsesPerMeter = 150;
const int encoderRightPin = 18;
const int encoderLeftPin = 19;

String USF = "";
String USR = "";
String IRFR = "";
String IRRR = "";
String IRRC = "";
String temp = "";

// ----------------------- //
// Instatiation of objects //
// ----------------------- //
Servo motor, steering;
unsigned long rcControllerFlag;
String input;
int steer, velocity, controlFlag;

//SONARS
SRF08 sonarFront;              
SRF08 sonarRight;

//INFRAREDS
GP2D120 irFrontRight;
GP2D120 irRearRight;
GP2D120 irRearCenter;

//WHEEL ENCODERS
Odometer encoderRight(pulsesPerMeter);
Odometer encoderLeft(pulsesPerMeter);

void setup() {
  //SERIAL CONNECTION
  Serial.begin(115200);

  motor.attach(escPin);
  motor.writeMicroseconds(1500);  // set motor to neutral
  steering.attach(servoPin);
  steering.write(90);  // set servo to neutral
  attachInterrupt(digitalPinToInterrupt(3), rcControllerInterrupt, RISING);

  rcControllerFlag = 0; // Set to 1 if an interupt comes from the RC controller
  controlFlag = 1;
  //SONARS
  sonarFront.attach(sonarFrontAddress);
  sonarRight.attach(sonarRightAddress);

  //INFRAREDS
  irFrontRight.attach(irFrontRightPin);
  irRearRight.attach(irRearRightPin);
  irRearCenter.attach(irRearCenterPin);

  //WHEEL ENCODERS
  encoderRight.attach(encoderRightPin);
  encoderRight.attach(encoderLeftPin);
}

void loop() {
  //rcControllerFlag = pulseIn(rcPinSteer, HIGH, 25000); // if the timeout is lower it sometimes time out before getting a value
  //Serial.print("Pulse read: ");
  //Serial.println(rcControllerFlag);
  sendSensorData();
  if(rcControllerFlag == 1){
    //rcControl();
    //controlFlag = 0;
    motor.writeMicroseconds(1500);
    steering.write(90);
    if (pulseIn(rcPinSteer, LOW, 25000) == 0){
      rcControllerFlag = 0;
      Serial.print("RC control set to off!");
    }
  }else if(controlFlag == 0){
    motor.writeMicroseconds(1500);
    steering.write(90);
    controlFlag = 1;
  }else{
    manualControl();
    //handleInput();
  }
}

void rcControl(){
  Serial.println("RC Control took over!");
  velocity = pulseIn(rcPinESC, HIGH, 25000);
  int i;
  int steerVals[10] = {90};
  for(i = 0; i < 10; i++){
    steerVals[i] = map(pulseIn(rcPinSteer, HIGH, 25000), 1000, 2000, 0, 180);;
  }
  steer = median(steerVals, 10) + 7;
  velocity = map(velocity, 1000, 1500, 0, 150);
  Serial.print("steer ");
  Serial.println(steer);
  Serial.print("velocity ");
  Serial.println(velocity);
  steering.write(steer);
  motor.writeMicroseconds(1650 - velocity);
  int temp = pulseIn(rcPinSteer, LOW, 25000);
  Serial.println(temp);
  if (pulseIn(rcPinSteer, LOW, 25000) == 0){
    rcControllerFlag = 0;
    Serial.print("RC control set to off!");
  }
  delay(100);
}

void manualControl(){
  if (Serial.available()){
    input = Serial.readStringUntil('\n');
    
    if (input.startsWith("t")){  // turning
      steer = input.substring(1).toInt();
      if (steer <= 180 && steer >=0){  // check that the value is in range
        steering.write(steer);
        Serial.println("Turning set to: ");
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

int median(int vals[], int len) {
  int minimum = vals[0];
  int maximum = vals[0];
  int sum = 0;
  int median = 90;
  for (int i = 0; i < len; i ++) {
    if (vals[i] < minimum) {
      minimum = vals[i];
    } else if (vals[i] > maximum) {
      maximum = vals[i];
    }
    sum += vals[i];
  }
  sum = sum - (minimum + maximum);
  median = floor(sum / (len-2));
  return median;
}

void rcControllerInterrupt(){
  rcControllerFlag = 1;
}

void sendSensorData(){
  USF = "USF ";
  USF.concat(sonarFront.getDistance());
  USR = " USR ";
  USR.concat(sonarRight.getDistance());
  IRFR = " IRFR ";
  IRFR.concat(irFrontRight.getDistance());
  IRRR = " IRRR ";
  IRRR.concat(irRearRight.getDistance());
  IRRC = " IRRC ";
  IRRC.concat(irRearCenter.getDistance());
  //String temp = "USF " + USF + " USR " + USR + " IRFR " + IRFR + " IRRR " + IRRR + " IRRC " + IRRC;
  //String netstring = encodedNetstring(USF + USR + IRFR + IRRR + IRRC);
  temp = USF + USR + IRFR + IRRR + IRRC;
  Serial.println(temp);
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

