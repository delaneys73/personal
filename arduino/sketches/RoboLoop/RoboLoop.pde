/*
 * simple sketch to write Robo Sapiens IR codes
 
 * write timespaced encoded data as descibed here:
 * http://www.aibohack.com/robosap/ir_codes.htm
 *
 * (approx 833us/ts, 8ts (1) as preamble, then each bit either 1/1 (0) of 4/1 (1).)
 */

const int irPin = 2;
const int tsDelay = 833; // us, as estimated

enum roboCommand {
  // only a very small subset of commands
  turnRight = 0x80,
  rightArmUp = 0x81,
  rightArmOut = 0x82,
  tiltBodyRight = 0x83,
  rightArmDown = 0x84,
  rightArmIn = 0x85,
  walkForward = 0x86,
  walkBackward = 0x87,
  turnLeft = 0x88,
  leftArmUp = 0x89,
  leftArmOut = 0x8A,
  tiltBodyLeft = 0x8B,
  leftArmDown = 0x8C,
  leftArmIn = 0x8D,
  stopMoving = 0x8E,
   
  // noises
  whistle = 0xCA,
  roar = 0xCE,
  burp = 0xC2 
};


void setup()
{
   pinMode(irPin, OUTPUT);
   digitalWrite(irPin, HIGH);     
}

void delayTs(unsigned int slices) {
  delayMicroseconds(tsDelay * slices); 
}

void writeCommand(int cmd) // this is type 'int' not 'roboCommand' due to an Arduino environment annoyance
{
  // preamble
  digitalWrite(irPin, LOW);
  delayTs(8);
    
  for(char b = 7; b>=0; b--) {
    digitalWrite(irPin, HIGH);
    delayTs( (cmd & 1<<b) ? 4 : 1 );
    digitalWrite(irPin, LOW);
    delayTs(1);        
  } 
  
  digitalWrite(irPin, HIGH);
}


void loop()
{
   writeCommand(leftArmUp);
   delay(2000);
   writeCommand(walkForward);
   delay(3000);
   writeCommand(rightArmUp);
   delay(2000);
   writeCommand(turnLeft);
   delay(3000);
   writeCommand(stopMoving);
   delay(500);
   writeCommand(burp);
   delay(3000);   
}
