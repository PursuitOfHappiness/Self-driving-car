#include "RemoteControl.h"

void rcControllerInterruptOn();
DrivingControl* serv;
int STEER_PIN;

RemoteControl::RemoteControl(int steer_pin, int esc_pin, DrivingControl *dc)
{
 serv = dc;
 STEER_PIN = steer_pin;
 attachInterrupt(digitalPinToInterrupt(3), rcControllerInterruptOn, RISING); // rc-controller turned on
  
}

/*
 * Listens for the interupts from the RC-Controller
 */
void rcControllerInterruptOn() {
    serv->motor.writeMicroseconds(1500);
    serv->steering.write(90);
    if (pulseIn(STEER_PIN, HIGH, 25000) == 0) { // if RC-Controller is turned off
    }
}
