// --------- //
// Libraries //
// --------- //
#include <Smartcar.h> //Including Dimitri's library

// --------- //
// Constants //
// --------- //
//MOTOR
const int servoPin = 5; //pin to which the servo motor is attached
const int escPin = 6; //pin to which the ESC is attached

//SONARS
unsigned short sonarFrontAddress = 0x73; //address to which the front sonar sensor is attached
unsigned short sonarRightAddress = 0x70;

//INFRAREDS
const int irFrontRightPin = 0;
const int irRearRightPin = 2;
const int irRearCenterPin  = 3;

//WHEEL ENCODERS
const int pulsesPerMeter = 150;
const int encoderRightPin = 7;
const int encoderLeftPin = 8;

// ----------------------- //
// Instatiation of objects //
// ----------------------- //
Car porsche(useServo(servoPin), useESC(escPin)); //instantiation of the car

//SONARS
SRF08 sonarFront;
SRF08 sonarRight;

//INFRAREDS
GP2Y0A21 irFrontRight;
GP2Y0A21 irRearRight;
GP2Y0A21 irRearCenter;

//WHEEL ENCODERS
Odometer encoderRight(pulsesPerMeter);
Odometer encoderLeft(pulsesPerMeter);


void setup() {
  // put your setup code here, to run once:

  porsche.begin();
  
  //SONARS
  sonarFront.attach(sonarFrontAddress);
  sonarRight.attach(sonarRightAddress);

  //INFRAREDS
  irFrontRight.attach(irFrontRightPin);
  irRearRight.attach(irRearRightPin);
  irRearCenter.attach(irRearCenterPin);

  //WHEEL ENCODERS
  encoderRight.attach(encoderRightPin);
  encoderRight.attach(encoderLeftPin);

  //SERIAL CONNECTION
  Serial.begin(9600);
  
  

}

void loop() {
  // put your main code here, to run repeatedly:
  printSonarFront();

  
  

}

void printSonarFront() {
  int distance = sonarFront.getDistance(); //get distance detected by sonarFront
  Serial.println(distance); // print the traveled distance
  delay(200); //run the above every 200 milliseconds
}

void printSonarRight() {
  int distance = sonarRight.getDistance(); //get distance detected by sonarRight
  Serial.println(distance); // print the traveled distance
  delay(200); //run the above every 200 milliseconds
}

void printIrFrontRight() {
  int distance = irFrontRight.getDistance(); //get distance detected by irFrontRight
  Serial.println(distance); // print the traveled distance
  delay(200); //run the above every 200 milliseconds
}

void printIrRearRight() {
  int distance = irRearRight.getDistance(); //get distance detected by irFrontRight
  Serial.println(distance); // print the traveled distance
  delay(200); //run the above every 200 milliseconds
}

void printIrRearCenter() {
  int distance = irRearCenter.getDistance(); //get distance detected by irFrontRight
  Serial.println(distance); // print the traveled distance
  delay(200); //run the above every 200 milliseconds
}




