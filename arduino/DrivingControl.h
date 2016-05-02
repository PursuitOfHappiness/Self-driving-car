#ifndef ARDUINO_DRIVING_CONTROL
#define ARDUINO_DRIVING_CONTROL

  #include <Wire.h>
  #include <Servo.h>
  #include "Arduino.h"

  #define RISING 3

class DrivingControl
{

  
  public:
    Servo motor, steering;
    void setSpeed(int speed);
    void setSteering(int steer);
    String getDistanceTraveled(char unit);
    DrivingControl(short servo_pin, short esc_pin, short wheel_pin);

    
};


#endif
