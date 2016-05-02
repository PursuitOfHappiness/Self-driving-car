#ifndef ARDUINO_REMOTE_CONTROL
#define ARDUINO_REMOTE_CONTROL

  #include <Wire.h>
  #include <Servo.h>
  #include "Arduino.h"
  #include "DrivingControl.h"


class RemoteControl
{

  public:
    RemoteControl(int steer_pin, int esc_pin, DrivingControl *dc);
    


  
};





#endif
