#include "InfraRed.h"
#include "Utils.h"

InfraRed::InfraRed(short adress)
{
  InfraRed::pin = adress;
  
}

/*
 * Calculates the distance an IR sensor is reporting. Returns the value as
 * centimeters. Returns 0 if the value is outside 5-25.
 * Takes an analog pin as input.
 */
int InfraRed::getDistance() {
  float volt = analogRead(InfraRed::pin);
  int cm = ((2914 / (volt + 5 )) - 1); // gives the range in centimeters
  values[0] = values[1];
  values[1] = values[2];
  values[2] = cm;
  if (cm >= 5 && cm <= 25) {
    return cm;
  }
  return -1; // if the value is not in our range
}

/*
 * Returns all 3 IR sensors value as a string.
 * " IRFR 'value' IRRR 'value' IRRC 'value'"
 */
String InfraRed::getIRData(String name) {
  String value = " " + name + " ";
  value.concat(Utils::fifo(values, getDistance(), 3));  // smooth values

  return value;
}
