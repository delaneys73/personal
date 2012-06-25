/* 
 Stepper Motor Control - Driving L298
 */

#include <Stepper.h>

const int stepsPerRevolution = 20;  // change this to fit the number of steps per revolution
                                    // for your motor
// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 9,10,11,12);            

int dir = 1;
int enable = 1;

void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(60);
  digitalWrite(13,HIGH);
  attachInterrupt(0,clock,RISING);
  attachInterrupt(1,chgDir,CHANGE);
  pinMode(4,INPUT);
  pinMode(3,INPUT);
}

void clock() {
  if (enable==HIGH)
  {
    myStepper.step(dir);
  }
}

void chgDir() {
  if (digitalRead(3)==HIGH))
  {  
    dir = 1;
  }
  else
  {
    dir = -1;
  }
}

void back() {
  dir = -1;
}

void loop() {
  // step one revolution  in one direction:
  enable = digitalRead(4);
}

