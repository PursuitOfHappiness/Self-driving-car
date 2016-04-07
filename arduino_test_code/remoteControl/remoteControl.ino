// --------- //
// Libraries //
// --------- //
#include <Servo.h> 
#include <Wire.h> // Sonars
#include <SonarSRF08.h> // Sonars
#include <Netstrings.h> // Include Dimitri's library for netstrings

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
#define FC_08_ADDRESS (0xE6 >> 1) // Front Center Sonar
#define FR_08_ADDRESS (0xE0 >> 1) // Front Right Sonar
#define GAIN_REGISTER 0x09 // Analog Gain
#define LOCATION_REGISTER 0x8C // 1 meter

char unit = 'c'; // 'i' for inches, 'c' for centimeters, 'm' for micro-seconds


//INFRAREDS  -- Analog
const int irFrontRightPin = 0;     //pin to which the front right infrared sensor is attached 
const int irRearRightPin = 1;      //pin to which the rear right infrared sensor is attached 
const int irRearCenterPin  = 2;    //pin to which the rear infrared sensor is attached 

//WHEEL ENCODERS
const int pulsesPerMeter = 150;
const int encoderRightPin = 18;
const int encoderLeftPin = 19;

// ----------------------- //
// Instatiation of objects //
// ----------------------- //
Servo motor, steering;
unsigned long rcControllerFlag;
String input;
int steer, velocity, controlFlag;

//SONARS
SonarSRF08 FrontCenterSonar;
SonarSRF08 FrontRightSonar;

void setup() {
  //SERIAL CONNECTION
  Serial.begin(115200);

  motor.attach(escPin);
  motor.writeMicroseconds(1500);  // set motor to neutral
  steering.attach(servoPin);
  steering.write(90);  // set servo to neutral
  attachInterrupt(digitalPinToInterrupt(3), rcControllerInterrupt, RISING);

  rcControllerFlag = 0; // Set to 1 if RC controller is turned on (interupt)
  controlFlag = 1; // Set to 0 when the RC takes over, used to set steering and speed to neutral when RC controller is turned off

  FrontCenterSonar.connect(FC_08_ADDRESS, GAIN_REGISTER, LOCATION_REGISTER);
  FrontRightSonar.connect(FR_08_ADDRESS, GAIN_REGISTER, LOCATION_REGISTER);
}

void loop() {
  //rcControllerFlag = pulseIn(rcPinSteer, HIGH, 25000); // if the timeout is lower it sometimes time out before getting a value
  //Serial.print("Pulse read: ");
  //Serial.println(rcControllerFlag);
  String US = getUSData();
  String IR = getIRData();
  Serial.println(US + IR);
  if(rcControllerFlag == 1){
    Serial.print("Interupted!");
    //rcControl();
    //controlFlag = 0;
    motor.writeMicroseconds(1500);
    steering.write(90);
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
  steer = pulseIn(rcPinSteer, HIGH, 25000);
//  int i;
//  int steerVals[10] = {90};
//  for(i = 0; i < 10; i++){
//    steerVals[i] = map(pulseIn(rcPinSteer, HIGH, 25000), 1000, 2000, 0, 180);;
//  }
//  steer = median(steerVals, 10) + 7;
  velocity = map(velocity, 1090, 2090, 0, 200);
  Serial.print("steer ");
  Serial.println(steer);
  Serial.print("velocity ");
  Serial.println(velocity);
  //steering.write(steer);
  if (velocity > 95 && velocity < 115){
    motor.writeMicroseconds(1500);
    //Serial.println(1500);
  } else if (velocity > 115){
    motor.writeMicroseconds(1550 + (velocity - 100));
    Serial.println(1550 + (velocity - 100));
  } else if (velocity < 95){
    motor.writeMicroseconds(1350 - (velocity + 100));
    Serial.println(1200 - (velocity + 100));
  }
  motor.writeMicroseconds(1650 - velocity);
  //int temp = pulseIn(rcPinSteer, LOW, 25000);
  //Serial.println(temp);
  if (pulseIn(rcPinSteer, HIGH, 25000) == 0){
    rcControllerFlag = 0;
    Serial.print("RC control set to off!");
  }
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

String getUSData(){
  String USF = " USF ";
  USF.concat(FrontCenterSonar.getRange(unit));
  String USR = " USR ";
  USR.concat(FrontRightSonar.getRange(unit));
  //String netstring = encodedNetstring(USF + USR + IRFR + IRRR + IRRC);
  String temp = USF + USR;
  return temp;
}

String getIRData(){
  String IRFR = " IRFR ";
  IRFR.concat(irCalc(irFrontRightPin));
  String IRRR = " IRRR ";
  IRRR.concat(irCalc(irRearRightPin));
  String IRRC = " IRRC ";
  IRRC.concat(irCalc(irRearCenterPin));
  String temp = IRFR + IRRR + IRRC;
  return temp;
}

int irCalc(int pin){
  float volt = analogRead(pin);
  int cm = ((2914 / (volt + 5 )) - 1);
  if (cm >= 5 && cm <= 25){
    return cm;
  }
  return -1;
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

