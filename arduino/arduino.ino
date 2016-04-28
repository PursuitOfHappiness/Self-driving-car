// --------- //
// Libraries //
// --------- //
#include <Servo.h>      // Steering and motor
#include <Wire.h>       // Sonars
#include <SonarSRF08.h> // Sonars

// --------- //
// Constants //
// --------- //
//SERVO
const int rcPinSteer = 3; // rc steering
const int rcPinESC = 9;  // rc motor

//MOTOR -- Digital
const int servoPin = 5;   // pin to which the servo motor is attached
const int escPin = 6;     // pin to which the ESC is attached

//SONARS  -- I2C
//Change these to Constants instead as it's preferred on the arduino
#define FC_08_ADDRESS (0xE6 >> 1) // Front Center Sonar
#define FR_08_ADDRESS (0xE0 >> 1) // Front Right Sonar
#define GAIN_REGISTER 0x07        // Analog Gain
#define LOCATION_REGISTER 0x8C    // 1 meter

char unit = 'c'; // 'i' for inches, 'c' for centimeters, 'm' for micro-seconds

//INFRAREDS  -- Analog
const int irFrontRightPin = 0;     // pin to which the front right infrared sensor is attached
const int irRearRightPin = 1;      // pin to which the rear right infrared sensor is attached
const int irRearCenterPin  = 2;    // pin to which the rear infrared sensor is attached

// ----------------------- //
// Instatiation of objects //
// ----------------------- //
Servo motor, steering;
volatile int wheelPulses, rcControllerFlag;
const int fifoSize = 3;             // Decides the size of the following arrays
int frCSArray[fifoSize] = {0};      // Front Center US
int frRSArray[fifoSize] = {0};      // Front Right US
int iRFRArray[fifoSize] = {0};      // Right Front IR
int iRRRArray[fifoSize] = {0};      // Right Rear IR
int iRRCArray[fifoSize] = {0};      // Rear Right Center IR
boolean newCommand = false;
String inputBuffer = "";

//SONARS
SonarSRF08 FrontCenterSonar;
SonarSRF08 FrontRightSonar;

void setup() {
  //SERIAL CONNECTION
  Serial.begin(57600);

  motor.attach(escPin);
  motor.writeMicroseconds(1500);  // set motor to neutral
  steering.attach(servoPin);
  steering.write(90);  // set servo to neutral
  attachInterrupt(digitalPinToInterrupt(3), rcControllerInterruptOn, RISING); // rc-controller turned on
  attachInterrupt(digitalPinToInterrupt(2), wheelPulse, RISING); // interupts from wheel encoder
  rcControllerFlag = 0; // Set to 1 if RC controller is turned on (interupt)
  wheelPulses = 0;

  // Setting up the sonars and limiting the range to 1 meter.
  FrontCenterSonar.connect(FC_08_ADDRESS, GAIN_REGISTER, LOCATION_REGISTER);
  FrontRightSonar.connect(FR_08_ADDRESS, GAIN_REGISTER, LOCATION_REGISTER);
}

void loop() {
  Serial.println(encodeNetstring(getUSData() + getIRData() + distTraveled())); // encode as a netstring and send over serial
  Serial.flush(); // wait for sending to be complete and clear outgoing buffer
  // Create a buffer with all the data that comes over the serial connection
  while (Serial.available() > 0){
    char c = Serial.read(); // Reads a value and removes it from the serial buffer
    inputBuffer += c;
    if (c == ','){
      newCommand = true;
    }
  }
  if (rcControllerFlag == 1) { // if an interupt is read from the RC-Controller
    motor.writeMicroseconds(1500);
    steering.write(90);
  } else if (newCommand){ // If a full command has been read form the serial communication
    int data[2];
    dataFromSerial(decodeNetstring(inputBuffer), data); // decode netstring, and extract data
    setSpeed(data[0]);
    setSteering(data[1]);
    inputBuffer = "";
    newCommand = false;
  }
}
/*
 * Listens for the interupts from the RC-Controller
 */
void rcControllerInterruptOn() {
  rcControllerFlag = 1;
}
/*
 * Listens for the interupts from the wheel
 */
void wheelPulse() {
  wheelPulses += 1;
}
/*
 * Returns both US sensors value as a string.
 * " USF 'value' USR 'value'"
 */
String getUSData() {
  String USF = "USF ";
  //USF.concat(FrontCenterSonar.getRange(unit));
  USF.concat(fifo(frCSArray, FrontCenterSonar.getRange(unit))); // smooth values
  String USR = " USR ";
  //USR.concat(FrontRightSonar.getRange(unit));
  USR.concat(fifo(frRSArray, FrontRightSonar.getRange(unit))); // smooth values

  return USF + USR;
}
/*
 * Returns all 3 IR sensors value as a string.
 * " IRFR 'value' IRRR 'value' IRRC 'value'"
 */
String getIRData() {
  String IRFR = " IRFR ";
  //IRFR.concat(irCalc(irFrontRightPin));
  IRFR.concat(fifo(iRFRArray, irCalc(irFrontRightPin)));  // smooth values
  String IRRR = " IRRR ";
  //IRRR.concat(irCalc(irRearRightPin));
  IRRR.concat(fifo(iRRRArray, irCalc(irRearRightPin))); // smooth values
  String IRRC = " IRRC ";
  //IRRC.concat(irCalc(irRearCenterPin));
  IRRC.concat(fifo(iRRCArray, irCalc(irRearCenterPin)));  // smooth values

  return IRFR + IRRR + IRRC;
}
/*
 * Calculates the distance the car has traveled in CM's
 */
String distTraveled() {
  String WP = " WP ";
  WP.concat(wheelPulses * 5);

  return WP;
}
/*
 * Calculates the distance an IR sensor is reporting. Returns the value as
 * centimeters. Returns 0 if the value is outside 5-25.
 * Takes an analog pin as input.
 */
int irCalc(int pin) {
  float volt = analogRead(pin);
  int cm = ((2914 / (volt + 5 )) - 1); // gives the range in centimeters
  if (cm >= 5 && cm <= 25) {
    return cm;
  }
  return 0; // if the value is not in our range
}
/*
 * Takes an array of integers as input and a new integer value
 * the new int value will be added and the oldest value of the
 * array will be removed. Oldest value is at the top.
 */
int fifo(int array[], int newValue) {
  int sum = 0;
  for (int i = 0; i < fifoSize - 1; i++) {
    array[i] = array[i + 1];
    sum += array[i + 1];
  }
  array[fifoSize - 1] = newValue;
  sum += newValue;
  return sum / fifoSize;
}
/*
 * Encoding netsrings. Takes a string and returns it as
 * '5:hello,'
 */
String encodeNetstring(String toEncode){
  String str = "";
  if (toEncode.length() < 1){
    return "Netstrings: Nothing to encode";
  }
  return String(toEncode.length()) + ":" + toEncode + ",";
}
/*
 * Decoding netsrings. Takes a string like this '5:hello'
 * and returns it like this 'hello'
 * Also checks that the netstring is of the correct format and size.
 */
String decodeNetstring(String toDecode){
  if (toDecode.length() < 3){ // A netstring can't be shorter than 3 characters
    return "Netstrings: Wrong format";
  }

  // Check that ':' and ',' exists at the proper places
  int semicolonIndex = toDecode.indexOf(':');
  int commaIndex = toDecode.lastIndexOf(',');
  if (semicolonIndex < 1 || commaIndex != toDecode.length() - 1) { // the first character has to be a number
    return "Netstrings: No semicolon found, or no comma found";
  }

  // Parse control number
  String number = toDecode.substring(0, semicolonIndex);
  int controlNumber = number.toInt();
  if (controlNumber < 1){ // the netstring has to contain atleast 1 character to be parsed
    return "Netstrings: Control Number is to low";
  }

  // Check that the length of the string is correct
  toDecode = toDecode.substring(semicolonIndex + 1, commaIndex); // the data that we want to parse
  if (controlNumber != toDecode.length()){
    return "Netstrings: Wrong length of data";
  }
  return toDecode;
}
/*
 * Decodes the string of data from Odroid. Takes a string and a pointer to an
 * int array with 2 values. Returns -1 as values if the string is malformed.
 * String must look like this: 'length':speed='int';angle='int';
 */
void dataFromSerial(String values, int *pdata){
  int equalSignIndexOne = values.indexOf('=');
  int equalSignIndexTwo = values.indexOf('=', equalSignIndexOne + 1);
  int semicolonIndexOne = values.indexOf(';');
  int semicolonIndexTwo = values.indexOf(';', semicolonIndexOne + 1);
  if (values.substring(0, equalSignIndexOne) == "speed" &&
        values.substring(semicolonIndexOne + 1, equalSignIndexTwo) == "angle"){
    pdata[0] = values.substring(equalSignIndexOne + 1 , semicolonIndexOne).toInt();
    pdata[1] = values.substring(equalSignIndexTwo + 1, semicolonIndexTwo).toInt();
  } else {
    pdata[0] = -1;
    pdata[1] = -1;
  }
}
/*
 * Function for speed. Checks that the value is inside the allowed range.
 */
void setSpeed(int speed){
  if (speed >= 1070 && speed <= 1620){
    motor.writeMicroseconds(speed);
  }
}
/*
 * Function for steering. Checks that the value is inside the allowed range.
 */
void setSteering(int steer){
  if (steer >= 60 && steer <= 130){
    steering.write(steer);
  }
}
