// --------- //
// Libraries //
// --------- //
#include <Smartcar.h> //Including Dimitri's library

// --------- //
// Constants //
// --------- //
//MOTOR -- Digital
const int servoPin = 5; //pin to which the servo motor is attached
const int escPin = 6; //pin to which the ESC is attached

//SONARS  -- I2C
unsigned short sonarFrontAddress = 0x73; //pin to which the front sonar sensor is attached 
unsigned short sonarRightAddress = 0x70; //pin to which the front right sonar sensor is attached

//INFRAREDS  -- Analog
const int irFrontRightPin = 0;     //pin to which the front right infrared sensor is attached 
const int irRearRightPin = 1;      //pin to which the rear right infrared sensor is attached 
const int irRearCenterPin  = 2;    //pin to which the rear infrared sensor is attached 

//WHEEL ENCODERS
const int pulsesPerMeter = 150;
const int encoderRightPin = 7;
const int encoderLeftPin = 8;

// ----------------------- //
// Instatiation of objects //
// ----------------------- //

//SONARS
SRF08 sonarFront;              
SRF08 sonarRight;

//INFRAREDS
GP2D120 irFrontRight;
GP2D120 irRearRight;
GP2D120 irRearCenter;

//WHEEL ENCODERS
Odometer encoderRight(pulsesPerMeter);
Odometer encoderLeft(pulsesPerMeter);


void setup() {
  
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
  //printSonarFront();
  //printSonarRight();
  //printIrFrontRight();
  //printIrRearRight();
  //printIrRearCenter();
  
}

void printSonarFront() {
  int distance = sonarFront.getDistance(); //get distance detected by sonarFront
  Serial.print("Sonar Front ");
  Serial.println(distance); // print the traveled distance
  delay(200); //run the above every 200 milliseconds
}

void printSonarRight() {
  int distance = sonarRight.getDistance();//get distance detected by sonarRight
   Serial.print("Sonar Right ");
  Serial.println(distance); // print the traveled distance
  delay(200); //run the above every 200 milliseconds
}

void printIrFrontRight() {
  int distance = irFrontRight.getDistance(); //get distance detected by irFrontRight
  Serial.print("IR Front Right ");
  Serial.println(distance); // print the traveled distance
  delay(200); //run the above every 200 milliseconds
}

void printIrRearRight() {
  int distance = irRearRight.getDistance(); //get distance detected by irFrontRight
  Serial.print("IR Rear Right ");
  Serial.println(distance); // print the traveled distance
  delay(200); //run the above every 200 milliseconds
}

void printIrRearCenter() {
  int distance = irRearCenter.getDistance(); //get distance detected by irFrontRight
  Serial.print("IR Center Rear ");
  Serial.println(distance); // print the traveled distance
  delay(200); //run the above every 200 milliseconds
}




