#ifndef ARDUINO_INFRA_RED
#define ARDUINO_INFRA_RED

  #include <Wire.h>
  #include <Servo.h>
  #include "Arduino.h"


class InfraRed
{

  public:
    InfraRed(short adress);
    int getDistance();
    int pin;
    String getIRData(String name);

  private:
    int values[3] = {0, 0, 0};

};





#endif
