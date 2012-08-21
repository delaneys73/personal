#include <Wire.h>

const char CMD_BLINK = 'b';
const char CMD_FADE = 'f';
const char CMD_RGBSET = 'r';
const char CMD_RGBFADE = 'x';

const int RED_PIN = 9;
const int GRN_PIN = 10;
const int BLU_PIN = 11;

int pins[] = {RED_PIN,GRN_PIN,BLU_PIN}; 

char currentCommand = CMD_BLINK;

// Common
int duration = 1000;
long lastRun = 0;

//Blink
int blink_curr_idx = 0;
//Fade
int fade_curr_idx = 0;
int fade_curr_level = 0;
//RGB Fade
int repeat = 0;     // How many times should we loop before stopping? (0 for no stop)
int rgbfade_prevR = 0;
int rgbfade_prevG = 0;
int rgbfade_prevB = 0;


void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
}

void loop()
{
  if (currentCommand==CMD_BLINK)
  {
      loopBlink();  
  }
  
  if (currentCommand==CMD_FADE)
  {
      loopFade();  
  }
}

void cmdRgbSet(int r,int g,int b)
{
  analogWrite(RED_PIN,r);
  analogWrite(GRN_PIN,g);
  analogWrite(BLU_PIN,b);  
  int rgbfade_prevR = r;
  int rgbfade_prevG = g;
  int rgbfade_prevB = b;
}

void blinkSetLast(int state)
{
     switch (blink_curr_idx){
       case 0:
         rgbfade_prevR = (state==LOW) ? 0 : 255;
         break;
       case 1:
         rgbfade_prevG = (state==LOW) ? 0 : 255;;
         break;         
       case 2:
         rgbfade_prevB = (state==LOW) ? 0 : 255;;
         break;         
     } 
}

void loopBlink()
{
  long now = millis();
   if ((lastRun+duration) < now)
   {
     lastRun = now;
     //Clear the current lights
     digitalWrite(pins[blink_curr_idx],LOW);
 
     blinkSetLast(LOW);
     blink_curr_idx++;
     if (blink_curr_idx>2)
     {
       blink_curr_idx = 0;
     }
     digitalWrite(pins[blink_curr_idx],HIGH);
     blinkSetLast(HIGH);     
   }
}

void loopFade()
{
  int fadeInterval = duration / 255;
  long now = millis();
   if ((lastRun+fadeInterval) < now)
   {
     lastRun = now;
     
     fade_curr_level ++;
     if (fade_curr_level>255)
     {
       digitalWrite(pins[fade_curr_idx],0);
       fade_curr_level = 0;
       fade_curr_idx ++;
       if (fade_curr_idx>2)
       {
         fade_curr_idx = 0;
       } 
     }
     
     analogWrite(pins[fade_curr_idx],fade_curr_level);
   }
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  currentCommand = Wire.read();
  
  if (currentCommand==CMD_BLINK)
  {
         //TODO Need to read as big int
    duration = Wire.read();
    Serial.print("Blink: ");
    Serial.println(duration);
  }
  
  if (currentCommand==CMD_FADE)
  {
         //TODO Need to read as big int
    duration = Wire.read();
    Serial.print("Fade: ");
    Serial.println(duration);
  }
  
  if (currentCommand==CMD_RGBSET)
  {
     int r = Wire.read(); 
     int g = Wire.read();
     int b = Wire.read();     
     cmdRgbSet(r,g,b);
  }
  
  if (currentCommand==CMD_RGBFADE)
  {
     int r = Wire.read(); 
     int g = Wire.read();
     int b = Wire.read();     
     //TODO Need to read as big int
     int wait = Wire.read();     
     int hold = Wire.read();  
     int colour[] = {r,g,b};   
     cmdRgbFade(colour,wait,hold);
  }
}

/* BELOW THIS LINE IS THE MATH -- YOU SHOULDN'T NEED TO CHANGE THIS FOR THE BASICS
* 
* The program works like this:
* Imagine a crossfade that moves the red LED from 0-10, 
*   the green from 0-5, and the blue from 10 to 7, in
*   ten steps.
*   We'd want to count the 10 steps and increase or 
*   decrease color values in evenly stepped increments.
*   Imagine a + indicates raising a value by 1, and a -
*   equals lowering it. Our 10 step fade would look like:
* 
*   1 2 3 4 5 6 7 8 9 10
* R + + + + + + + + + +
* G   +   +   +   +   +
* B     -     -     -
* 
* The red rises from 0 to 10 in ten steps, the green from 
* 0-5 in 5 steps, and the blue falls from 10 to 7 in three steps.
* 
* In the real program, the color percentages are converted to 
* 0-255 values, and there are 1020 steps (255*4).
* 
* To figure out how big a step there should be between one up- or
* down-tick of one of the LED values, we call calculateStep(), 
* which calculates the absolute gap between the start and end values, 
* and then divides that gap by 1020 to determine the size of the step  
* between adjustments in the value.
*/

int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero, 
    step = 1020/step;              //   divide by 1020
  } 
  return step;
}

/* The next function is calculateVal. When the loop value, i,
*  reaches the step size appropriate for one of the
*  colors, it increases or decreases the value of that color by 1. 
*  (R, G, and B are each calculated separately.)
*/

int calculateVal(int step, int val, int i) {

  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;           
    } 
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    } 
  }
  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  } 
  else if (val < 0) {
    val = 0;
  }
  return val;
}

/* crossFade() converts the percentage colors to a 
*  0-255 range, then loops 1020 times, checking to see if  
*  the value needs to be updated each time, then writing
*  the color values to the correct pins.
*/

void cmdRgbFade(int color[3],int wait,int hold) {
  // Convert to 0-255
  int R = color[0];
  int G = color[1];
  int B = color[2];

  int stepR = calculateStep(rgbfade_prevR, R);
  int stepG = calculateStep(rgbfade_prevG, G); 
  int stepB = calculateStep(rgbfade_prevB, B);
  
  int redVal = rgbfade_prevR;
  int grnVal = rgbfade_prevG;
  int bluVal = rgbfade_prevB;

  for (int i = 0; i <= 1020; i++) {
    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);

    analogWrite(RED_PIN, redVal);   // Write current values to LED pins
    analogWrite(GRN_PIN, grnVal);      
    analogWrite(BLU_PIN, bluVal); 

    delay(wait); // Pause for 'wait' milliseconds before resuming the loop
  }
  // Update current values for next loop
  rgbfade_prevR = redVal; 
  rgbfade_prevG = grnVal; 
  rgbfade_prevB = bluVal;
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}
