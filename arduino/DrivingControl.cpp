#include "DrivingControl.h"

void wheelPulse();
volatile int rotations = 0;

DrivingControl::DrivingControl(short servo_pin, short esc_pin, short wheel_pin)
{
  rotations = 0;
  attachInterrupt(digitalPinToInterrupt(wheel_pin), wheelPulse, RISING); // interupts from wheel encoder
  motor.attach(esc_pin);
  motor.writeMicroseconds(1500);  // set motor to neutral
  steering.attach(servo_pin);
  steering.write(90);  // set servo to neutral
  
}

/*
 * Function for speed. Checks that the value is inside the allowed range.
 */
void DrivingControl::setSpeed(int speed){
  if (speed >= 1070 && speed <= 1620){
    motor.writeMicroseconds(speed);
  }
}

/*
 * Function for steering. Checks that the value is inside the allowed range.
 */
void DrivingControl::setSteering(int steer){
  if (steer >= 60 && steer <= 130){
    steering.write(steer);
  }
}

/*
 * Calculates the distance the car has traveled in it's given unit. (M is millimeters because I can't find out how microseconds would work).
 */
String DrivingControl::getDistanceTraveled(char unit) {
  String WP = " WP ";
  if(unit == 'c')
    WP.concat(rotations * 5);
  else if(unit == 'i')
    WP.concat(rotations * 2);
  else if(unit == 'm')
    WP.concat(rotations * 50);
  else
    WP.concat(-1);

  return WP;
}


/*
 * Listens for the interupts from the wheel
 */
void wheelPulse() {
  rotations += 1;
}


