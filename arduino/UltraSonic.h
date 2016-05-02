#ifndef ARDUINO_ULTRA_SONIC
#define ARDUINO_ULTRA_SONIC

  #include <Wire.h>
  #include <Servo.h>
  #include "Arduino.h"

  #define GAIN_REGISTER           0x05        // Analog Gain
  #define LOCATION_REGISTER       0x18        // 1 meter


class UltraSonic
{

  public:
    UltraSonic(short adress);
    int getDistance();
    int pin;
    String getUSData(String name);

  private:
    int rangingNotDone();
    int values[3] = {0, 0, 0};

  
};





#endif
