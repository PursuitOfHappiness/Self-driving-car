#include "RemoteControl.h"

void rcControllerInterruptOn();
DrivingControl* serv;

RemoteControl::RemoteControl(DrivingControl *dc)
{
 serv = dc;
 attachInterrupt(digitalPinToInterrupt(3), rcControllerInterruptOn, RISING); // rc-controller turned on
  
}

/*
 * Listens for the interupts from the RC-Controller
 */
void rcControllerInterruptOn() {
    serv->motor.writeMicroseconds(1500);
    serv->steering.write(90);
    if (pulseIn(3, HIGH, 25000) == 0) { // if RC-Controller is turned off
    }
}
