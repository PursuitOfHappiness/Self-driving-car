#include "UltraSonic.h"
#include "Utils.h"


UltraSonic::UltraSonic(short adress)
{

  UltraSonic::pin = adress;

  Wire.begin();
  // Setting up the sonars and limiting the range to 1 meter.

  Wire.beginTransmission(adress);
  Wire.write(0x00);
  Wire.write(GAIN_REGISTER);
  Wire.write(LOCATION_REGISTER);
  Wire.endTransmission();
  
}
/*
 * Calculates the distance a US sensor is reporting. Returns the value as
 * centimeters. Returns 0 if the value is outside 5-90.
 * Takes an int corresponding to a US sensor as input.
 */
int UltraSonic::getDistance() {
  int range;
  Wire.beginTransmission(UltraSonic::pin);
  Wire.write(0x00);                      // First adress, used for range
  Wire.write(0x51);                      // Send 0x51 to start a ranging in cm
  Wire.endTransmission();
  rangingNotDone();
  //delay(100);

  Wire.beginTransmission(UltraSonic::pin);    // start communicating with SRFmodule
  Wire.write(0x02);              // Call the register for start of ranging data
  Wire.endTransmission();

  Wire.requestFrom(UltraSonic::pin, 2);           // Request 2 bytes from SRF module
  while(Wire.available() < 2);                 // Wait for data to arrive
  byte highByte = Wire.read();                 // Get high byte
  byte lowByte = Wire.read();                  // Get low byte
  range = (highByte << 8) + lowByte;           // Put them together

  values[0] = values[1];
  values[1] = values[2];
  values[2] = range;

  if (range >= 5 && range <= 90) {
    return range;
  }
  return -1; // if the value is not in our range
}
int UltraSonic::rangingNotDone(){
  int done = -1;
  Wire.beginTransmission(UltraSonic::pin);    // Begin communication with the SRF module
  Wire.write(0x00);                // Sends the command bit, when this bit is read it returns the software revision
  Wire.endTransmission();
  while (done == -1){
    Wire.requestFrom(UltraSonic::pin, 1); // Request 1 byte
    while (Wire.available() < 0); // While byte available
    done = Wire.read(); // Get byte
    delay(1);
  }
  return 1;
}

/*
 * Returns both US sensors value as a string.
 * " USF 'value' USR 'value'"
 */
String UltraSonic::getUSData(String name) {
  String value =  " " + name + " ";
  value.concat(Utils::fifo(values, getDistance(), 3)); // smooth values

  return value;
}
