// --------- //
// Libraries //
// --------- //
#include <Servo.h>      // Steering and motor
#include <Wire.h>       // Sonars

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
#define GAIN_REGISTER 0x0B        // Analog Gain
#define LOCATION_REGISTER 0x18    // 1 meter

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
int speedReference, referenceCount;
int frCSArray[fifoSize] = {0};      // Front Center US
int frRSArray[fifoSize] = {0};      // Front Right US
int iRFRArray[fifoSize] = {0};      // Right Front IR
int iRRRArray[fifoSize] = {0};      // Right Rear IR
int iRRCArray[fifoSize] = {0};      // Rear Right Center IR
boolean newCommand = false;
String inputBuffer = "";
String encodedToSend = "";
String decodedFromOdroid = "";

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
  speedReference = 1500;
  referenceCount = 0;
  Wire.begin();
  // Setting up the sonars and limiting the range to 1 meter.

  Wire.beginTransmission(FC_08_ADDRESS);
  Wire.write(0x00);
  Wire.write(GAIN_REGISTER);
  Wire.write(LOCATION_REGISTER);
  Wire.endTransmission();

  Wire.beginTransmission(FR_08_ADDRESS);
  Wire.write(0x00);
  Wire.write(GAIN_REGISTER);
  Wire.write(LOCATION_REGISTER);
  Wire.endTransmission();

}

void loop() {
  if (encodeNetstring(getUSData() + getIRData() + distTraveled()) == 1){
    Serial.println(encodedToSend); // encode as a netstring and send over serial
    encodedToSend = "";
  }
  //Serial.flush(); // wait for sending to be complete and clear outgoing buffer
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
    if (pulseIn(rcPinSteer, HIGH, 25000) == 0) { // if RC-Controller is turned off
      rcControllerFlag = 0;
    }
  } else if (newCommand){ // If a full command has been read form the serial communication
    int data[2];
    if (decodeNetstring(inputBuffer) == 1){
      Serial.println("decode netstring correct");
      dataFromSerial(decodedFromOdroid, data); // decode netstring, and extract data
      if (data[0] == 1500){
        setSpeed(1500);
        speedReference = 1500;
      } else if (data[0] != speedReference && data[0] > 1500){ // we get a new speed moving forward
        speedReference = data[0];
        setSpeed(1700); // set the speed high to kickstart the car
        Serial.println("Set speed 1700");
        int wheelPulsesTemp = wheelPulses;
        while(wheelPulses < wheelPulsesTemp + 4){
          // do nothing
        }
        setSpeed(1590);// lower the speed again
        Serial.println("Set speed 1600");
      } else if (data[0] != speedReference && data[0] < 1500) { // we get a new speed moving backwards
        speedReference = data[0];
        setSpeed(1000); // set the speed high to kickstart the car
        Serial.println("Set speed 1000");
        int wheelPulsesTemp = wheelPulses;
        while(wheelPulses < wheelPulsesTemp + 4){
          // do nothing
        }
        setSpeed(1100); // lower the speed again
        Serial.println("Set speed 1100");
      }
      setSteering(data[1]);
    }
    inputBuffer = "";
    newCommand = false;
    decodedFromOdroid = "";
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
  USF.concat(fifo(frCSArray, usCalc(FC_08_ADDRESS))); // smooth values
  String USR = " USR ";
  //USR.concat(FrontRightSonar.getRange(unit));
  USR.concat(fifo(frRSArray, usCalc(FR_08_ADDRESS))); // smooth values

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
  return -1; // if the value is not in our range
}
/*
 * Calculates the distance a US sensor is reporting. Returns the value as
 * centimeters. Returns 0 if the value is outside 5-90.
 * Takes an int corresponding to a US sensor as input.
 */
int usCalc(int sensorAddress) {
  int range;
  Wire.beginTransmission(sensorAddress);
  Wire.write(0x00);                      // First adress, used for range
  Wire.write(0x51);                      // Send 0x51 to start a ranging in cm
  Wire.endTransmission();
  rangingNotDone(sensorAddress);
  //delay(100);

  Wire.beginTransmission(sensorAddress);    // start communicating with SRFmodule
  Wire.write(0x02);              // Call the register for start of ranging data
  Wire.endTransmission();

  Wire.requestFrom(sensorAddress, 2);           // Request 2 bytes from SRF module
  if (Wire.available() < 2){      // If there is not enough data return -1
    return -1;
  }
  byte highByte = Wire.read();                 // Get high byte
  byte lowByte = Wire.read();                  // Get low byte
  range = (highByte << 8) + lowByte;           // Put them together
  if (range >= 5 && range <= 90) {
    return range;
  }
  return -1; // if the value is not in our range
}
int rangingNotDone(int sensorAddress){
  int done = -1;
  Wire.beginTransmission(sensorAddress);    // Begin communication with the SRF module
  Wire.write(0x00);                // Sends the command bit, when this bit is read it returns the software revision
  Wire.endTransmission();
  while (done == -1){
    Wire.requestFrom(sensorAddress, 1); // Request 1 byte
    while (Wire.available() < 0); // While byte available
    done = Wire.read(); // Get byte
    delay(1);
  }
  return 1;
}
/*
 * Takes an array of integers as input and a new integer value
 * the new int value will be added and the oldest value of the
 * array will be removed. Oldest value is at the top.
 */
int fifo(int array[], int newValue) {
  int sum = 0;
  int divideby = 0;
  for (int i = 0; i < fifoSize - 1; i++) { // mvoe the values around in the queue
    array[i] = array[i + 1];
    if (array[i + 1] > 0){ // smoothing is only done with real ranges
      sum += array[i + 1];
      divideby++;
    }
  }
  array[fifoSize - 1] = newValue;
  if (newValue > 0){
    sum += newValue;
    divideby++;
  }
  if (divideby < 3){ // less than 3 values is treated as ghost values
    return -1;
  }
  return sum / divideby;
}
/*
 * Encoding netsrings. Takes a string and returns it as
 * '5:hello,'
 */
int encodeNetstring(String toEncode){
  if (toEncode.length() < 1){
    return -1;
  }
  encodedToSend = String(toEncode.length()) + ":" + toEncode + ",";
  return 1;
}
/*
 * Decoding netsrings. Takes a string like this '5:hello'
 * and returns it like this 'hello'
 * Also checks that the netstring is of the correct format and size.
 */
int decodeNetstring(String toDecode){
  if (toDecode.length() < 3){ // A netstring can't be shorter than 3 characters
    return -1;
  }

  // Check that ':' and ',' exists at the proper places
  int semicolonIndex = toDecode.indexOf(':');
  int commaIndex = toDecode.lastIndexOf(',');
  if (semicolonIndex < 1 || commaIndex != toDecode.length() - 1) { // the first character has to be a number
    return -1;
  }

  // Parse control number
  String number = toDecode.substring(0, semicolonIndex);
  int controlNumber = number.toInt();
  if (controlNumber < 1){ // the netstring has to contain atleast 1 character to be parsed
    return -1;
  }

  // Check that the length of the string is correct
  toDecode = toDecode.substring(semicolonIndex + 1, commaIndex); // the data that we want to parse
  if (controlNumber != toDecode.length()){
    return -1;
  }
  decodedFromOdroid = toDecode;
  return 1;
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
  if (speed >= 1000 && speed <= 1700){
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
