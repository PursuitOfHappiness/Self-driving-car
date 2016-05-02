//-----------//
// Libraries //
//-----------//
#include <Servo.h>
#include <Wire.h>
#include "DrivingControl.h"
#include "InfraRed.h"
#include "UltraSonic.h"
#include "RemoteControl.h"


//-----------//
// Constants //
//-----------//

// Servo //
#define REMOTE_PIN_STEER        3           // Steering pin
#define REMOTE_PIN_ESC          9           // esc Pin

// Motor //
#define SERVO_PIN               5           // Servo pin
#define ESC_PIN                 6           // esc pin
#define WHEEL_SENSOR_PIN        2

// Sonars //
#define FC_08_ADDRESS           (0xE6 >> 1) // Front Center Sonar
#define FR_08_ADDRESS           (0xE0 >> 1) // Front Right Sonar

//  Infrareds //
#define IR_FRONT_RIGHT_PIN      0x00        // Front ir sensor
#define IR_REAR_RIGHT_PIN       0x01        // Rear right ir sensor
#define IR_REAR_CENTER_PIN      0x02        // Rear center ir sensor

// Other //
#define UNIT                    'c'         // 'i' for inches, 'c' for centimeters, 'm' for micro-seconds
#define BAUD_RATE               57600       //  The baudrate for sending and receiving commands


//------------//
// Initiation //
//------------//
DrivingControl* dc;
InfraRed* ir_fr;
InfraRed* ir_rr;
InfraRed* ir_rc;
UltraSonic* us_c;
UltraSonic* us_r;
RemoteControl* rc;
String inputBuffer = "";
bool newCommand = false;




void setup()
{

  dc = new DrivingControl(SERVO_PIN, ESC_PIN, WHEEL_SENSOR_PIN);
  ir_fr = new InfraRed(IR_FRONT_RIGHT_PIN);
  ir_rr = new InfraRed(IR_REAR_RIGHT_PIN);
  ir_rc = new InfraRed(IR_REAR_CENTER_PIN);
  us_c = new UltraSonic(FC_08_ADDRESS);
  us_r = new UltraSonic(FR_08_ADDRESS);
  // Setup the serial connection with correct baud rate//
  Serial.begin(BAUD_RATE);
  
  
}


void loop()
{
Serial.println(encodeNetstring(us_c->getUSData("USC") + us_r->getUSData("USR") + ir_fr->getIRData("IRFR") + ir_rr->getIRData("IRRR") + ir_rc->getIRData("IRRC") + dc->getDistanceTraveled(UNIT))); // encode as a netstring and send over serial
  Serial.flush(); // wait for sending to be complete and clear outgoing buffer
  // Create a buffer with all the data that comes over the serial connection
  while (Serial.available() > 0){
    char c = Serial.read(); // Reads a value and removes it from the serial buffer
    inputBuffer += c;
    if (c == ','){
      newCommand = true;
    }
  }

  if (newCommand){ // If a full command has been read form the serial communication
    int data[2];
    dataFromSerial(decodeNetstring(inputBuffer), data); // decode netstring, and extract data
    dc->setSpeed(data[0]);
    dc->setSteering(data[1]);
    inputBuffer = "";
    newCommand = false;
  }

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



