//Delaney Christmas Lights Controller
// _SHIFTPWM_ to use ShiftPWM, Which is standard 74HC595 shift registers over SPI / with PWM
// _SHIFTOUT_ to use standard 74HC595 shift registers directly, No PWM
// _TLC5940_ to use TLC5940 IC with PWM
#define _SHIFTOUT_
#define _SLAVE_

#include <Wire.h>
#include <EEPROM.h>

extern int __bss_end;
extern void *__brkval;

#if defined _SHIFTPWM_
#include <SPI.h>
//Data pin is MOSI (atmega168/328: pin 11. Mega: 51) 
//Clock pin is SCK (atmega168/328: pin 13. Mega: 52)
const int ShiftPWM_latchPin=8;
const bool ShiftPWM_invertOutputs = 0; // if invertOutputs is 1, outputs will be active low. Usefull for common anode RGB led's.
#include <ShiftPWM.h>   // include ShiftPWM.h after setting the pins!
const int lightsPerRegister = 8;
const int maxLights = 64;
const int FULL_INTENSITY=255;
//Constants
const int SPWM_FREQUENCY = 75;
#endif

#if defined _SHIFTOUT_
//Standard shift register code
const int dataPin = 11;
const int clockPin = 13;
const int latchPin = 8;
const int lightsPerRegister = 8;
const int maxLights = 128;
const int FULL_INTENSITY=255;
byte data[16];
#endif

#if defined _TLC5940_
#include "Tlc5940.h"
//TLC5940 PWM Shift Register
const int lightsPerRegister = 16;
const int maxLights = 128;
const int FULL_INTENSITY=4095;
#endif

const int NO_INTENSITY=0;
const int ON=FULL_INTENSITY;
const int OFF=0;
const int ADDR_CONN_LIGHTS = 1;
const int ADDR_NUM_REGISTERS = 2;
const int ADDR_AUTO_INTERVAL = 3;
const int ADDR_AUTO_DURATION = 5;
const int ADDR_ID = 0;

const byte EEPROM_ID = 0x99;

//Read from EEPROM
int connectedLights = 8;
int numRegisters = 1;
long autoCycleDuration = 60000;

//For beat HW macro
int mSF_Interval = 0;
int mSF_Pin = -1;
unsigned long mSF_LastBeat = 0;
boolean mSF_On = false;

#if defined _MASTER_
const int DIR_FWD = 1;
const int DIR_BK = -1;

const String CMD_RESET = "RE";
const String CMD_AUTO = "AU";
const String CMD_AUTO_DURATION = "AD";
const String CMD_MANUAL = "MN";
const String CMD_LIGHT = "LI";
const String CMD_BEAT = "BE";
const String CMD_OUTPUTLINES = "OL";
const String CMD_CONFIG = "CF";

//Common
char command[10];
int idx = 0;

//Auto mode variables
boolean autoMode = true;
int autoInterval = 20;
#if defined _SHIFTOUT_
int autoIntervals[] = {3000,100,100,500,3000};
#else
int autoIntervals[] = {20,100,100,500,20};
#endif

unsigned long autoLast = 0;

//Fade Off variables
int autoLightIndex = 0;
int autoCurrLevel = 255;
int autoIncr = -5;
int autoCycleIndex = 0;
int autoCycles = 0;
boolean autoChangeCycle = false;

//Alternate variables
boolean alternateToggle = false;

//Fade All variables
int fadeAllDirection =  DIR_FWD;
int fadeAllLevel = NO_INTENSITY;

//For Knight Rider Auto Mode
int mKR_Direction= DIR_FWD;
int mKR_Channel=0;
#endif

#if defined _SLAVE_
byte address = 8;
const int ADDR_PIN1 = 6;
const int ADDR_PIN2 = 7;
const int ADDR_PIN3 = 8;

const byte RCMD_LIGHT = 0x01;
const byte RCMD_ALL = 0x01;
const byte RCMD_OUTPUTLINES = 0x02;
#endif

void setup() {
  //Power for test board  

  Serial.begin(38400);
  #if defined _MASTER_  
    Wire.begin();
    Serial.println(F("Delaney Xmas Firmware - Start v4"));
  #else
    setupAddress();
    Wire.begin(address);
    Wire.onReceive(receiveData);
    Serial.println(F("Delaney Xmas Firmware - Start v4 (Slave)"));
  #endif
  
  readSettings();
  setupRegisters();
  
  randomSeed(analogRead(0));
  
  reset();
}

//Read current values from EEPROM
void readSettings()
{
  byte id = EEPROM.read(ADDR_ID);
  if (id == EEPROM_ID)
  {
     Serial.println(F("Loading stored config from EEPROM"));
     //Valid data
     connectedLights = EEPROM.read(ADDR_CONN_LIGHTS);
     numRegisters = EEPROM.read(ADDR_NUM_REGISTERS);
     byte hb2 = EEPROM.read(ADDR_AUTO_DURATION);
     byte lb2 = EEPROM.read(ADDR_AUTO_DURATION+1);
     autoCycleDuration = word(hb2,lb2);
     
  } 
  else
  {
     Serial.println(F("No valid EEPROM config found"));
     saveSettings();
  }
}

int calculateNumRegisters()
{
  return ((connectedLights % lightsPerRegister) == 0) ? (connectedLights / lightsPerRegister)  : (connectedLights / lightsPerRegister) +1;
}

void saveSettings()
{
   Serial.println(F("Saving config to EEPROM"));
   EEPROM.write(ADDR_ID,EEPROM_ID);
   EEPROM.write(ADDR_CONN_LIGHTS,connectedLights);
   numRegisters = calculateNumRegisters();
   EEPROM.write(ADDR_NUM_REGISTERS,numRegisters); 
   byte hb2 = highByte(autoCycleDuration);
   byte lb2 = lowByte(autoCycleDuration);
   EEPROM.write(ADDR_AUTO_DURATION,hb2);
   EEPROM.write(ADDR_AUTO_DURATION+1,lb2);      
   setAmountOfRegisters(numRegisters);
}

int memoryFree()
{
  int freeValue;
  if ((int)__brkval==0)
  {
    freeValue = ((int)&freeValue)- ((int)&__bss_end);
  }
  else
  {
    freeValue = ((int)&freeValue) - ((int)__brkval);
  } 
  
  return freeValue;
}

/**
 * Macro flashes one light off for the specified pin and the defined interval
 */
void macroSteadyFlash()
{
  if (mSF_Pin!=-1)
  {
    if ((mSF_LastBeat+mSF_Interval)< millis())
    {
      mSF_LastBeat = millis();  
      mSF_On = !mSF_On;

      if (mSF_On==false)
      {
        setOutput(mSF_Pin, LOW,0);
      }
      else
      {
        setOutput(mSF_Pin, HIGH,0);
      }
    }
  } 
}


#if defined _SLAVE_
void loop()
{
  
}

void setupAddress()
{
    pinMode(ADDR_PIN1,INPUT);
    pinMode(ADDR_PIN2,INPUT);
    pinMode(ADDR_PIN3,INPUT);
    bitWrite(address,2, digitalRead(ADDR_PIN1));
    bitWrite(address,1, digitalRead(ADDR_PIN2));
    bitWrite(address,0, digitalRead(ADDR_PIN3));
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveData(int howMany)
{
  byte cmd = Wire.read();
  
  if (cmd==RCMD_LIGHT)
  {
     int lightNo = Wire.read();
     byte intensityHigh  = Wire.read();     
     byte intensityLow  = Wire.read();
     long intensity = word(intensityHigh,intensityLow);
     setOne(lightNo,intensity);
  }

  if (cmd==RCMD_ALL)
  {
     byte intensityHigh  = Wire.read();     
     byte intensityLow  = Wire.read();
     long intensity = word(intensityHigh,intensityLow);
     setAll(intensity);
  }
  
  if (cmd==RCMD_OUTPUTLINES)
  {
     int olines  = Wire.read();
    connectedLights = constrain(olines,1,64);
    saveSettings();
    setAmountOfRegisters(numRegisters);         
  }
}
#endif

#if defined _MASTER_
void loop() 
{
  // Process commands from serial
  if (Serial.available() > 0) 
  {
     readCommand();
  } 

  if (autoMode==true)
  {
     processAuto();
  }
  else
  {
    macroSteadyFlash(); 
  }
}

void readCommand()
{
   for (int i=0; i < Serial.available(); i++){
      command[idx] = Serial.read();
      if (command[idx]=='\n')
      {
        processCommand();        
        idx=0;
      }
      else
      {
        idx++;
      }
    }  
}

void processAuto()
{
    if (autoChangeCycle)
    {
      autoChangeCycle = false;
      autoCycleIndex ++;
      if (autoCycleIndex >= 5)
      {
        autoCycleIndex = 0;
      }      
      autoInterval = autoIntervals[autoCycleIndex];
    }
    
    long now = millis();
    if (autoLast+autoInterval < now)
    {
      
      if (autoCycleIndex==0)
      {
        #if defined _SHIFTOUT_
          autoOneByOne();
        #else
          autoFadeOff();
        #endif
      }
      
      if (autoCycleIndex==1)
      {    
          autoKnightRider();
      }
  
      if (autoCycleIndex==2)
      {
          autoTwinkle();
      }
  
      if (autoCycleIndex==3)
      {
          autoAlternate();
      }
  
      if (autoCycleIndex==4)
      {
        #if defined _SHIFTOUT_
          autoOneByOne();
        #else
          autoFadeAll();
        #endif
      } 
      autoLast = now;
      autoCycles ++;
      if (autoCycles >= (autoCycleDuration/autoInterval))
      {
        autoChangeCycle = true;      
        autoCycles = 0;
      }
    }
}


/**
 *  Commands are 2 char codes followed by up to 7 bytes of data
 */
void processCommand() {
  String op = "  ";

  if (sizeof(command) >=2)
  {
    op.setCharAt(0,command[0]);
    op.setCharAt(1,command[1]);
  }

  if (op.equals(CMD_CONFIG))
  {
     Serial.println(F("CONFIG\n======"));
     Serial.print(F("Shift Registers: "));
     Serial.println(numRegisters);
     Serial.print(F("Connected Lights: "));
     Serial.println(connectedLights);
     Serial.print(F("Auto Mode Interval: "));
     Serial.println(autoInterval);
     Serial.print(F("Auto Cycle Duration: "));
     Serial.println(autoCycleDuration);     
     Serial.println(F("LOAD\n===="));     
     printLoad();  
     Serial.println(F("MEMORY\n======"));
     Serial.print(F("Free: "));
     Serial.println(memoryFree());       
  }


  if (op.equals(CMD_RESET))
  {
    reset();
  }

  //Turn On Auto Mode
  if (op.equals(CMD_AUTO)) 
  {
    autoLightIndex = 0;
    autoMode = true;
    autoInterval = constrain(long(readInt(command,2,5)),200,10000);
    saveSettings();
    reset();
  }

  if (op.equals(CMD_AUTO_DURATION)) 
  {
    autoLightIndex = 0;
    autoMode = true;
    autoCycleDuration = constrain(long(readInt(command,2,6)),10000,60000);
    saveSettings();
    reset();
  }

  //Switch to Manual Mode
  if (op.equals(CMD_MANUAL))
  {
    autoMode = false;
    reset();
  }

  //Change the state of an output (light) as represented in the shift data
  if (op.equals(CMD_LIGHT))
  {
    int outputNo = readInt(command,2,4);
    int state = readInt(command,5,5);
    // write to the shift register with the correct bit set high:
    setOutput(outputNo,state,0);
  }

  if (op.equals(CMD_BEAT))
  {
    mSF_Interval = long(readInt(command,2,5));
    mSF_Pin = readInt(command,6,6);
  }

  if (op.equals(CMD_OUTPUTLINES))
  {
    connectedLights = constrain(readInt(command,2,5),1,64);
    saveSettings();
  }
}

void autoAlternate() {
    setAll(OFF);
    for (int x=0;x < numRegisters;x++)
    { 
      for (int y=0;y < lightsPerRegister;y++)
      {
        int pin = (x * lightsPerRegister) + y;
        
        int rem = pin % 2;
        
        if ( (rem==0 && alternateToggle==true) || (rem !=0 && alternateToggle==false))
        {
           setOne(pin,ON);
        }
        else
        {
           setOne(pin,OFF);
        }
      }
    }
    alternateToggle = !alternateToggle;
}

void autoTwinkle() {
    setAll(OFF);
    for (int x=0;x < numRegisters;x++)
    {
      byte rnum = random(FULL_INTENSITY);
      for (int y=0;y < lightsPerRegister;y++)
      {
        int val = bitRead(rnum,y);
        int pin = (x * lightsPerRegister) + y;
        if (val==1)
        {
          int rbn = random(FULL_INTENSITY);
          setOne(pin,rbn);
        }
        else
        {
          setOne(pin,OFF);
        }
      }
    } 
}

void autoKnightRider()
{
      setAll(OFF);

      if (mKR_Channel == 0) 
      {
        mKR_Direction =  DIR_FWD;
      } 
      else 
      {
         setOne(mKR_Channel - 1, 20);
      }

      setOne(mKR_Channel, ON);

      if (mKR_Channel != connectedLights - 1) 
      {
        setOne(mKR_Channel + 1, 20);
      } 
      else 
      {
        mKR_Direction =  DIR_BK;
      }

      mKR_Channel += mKR_Direction;
}

#if defined _SHIFTOUT_
void autoOneByOne()
{    
    setAll(ON);
    setOne(autoLightIndex , OFF);  
    autoLightIndex ++;
    
    if (autoLightIndex >(connectedLights-1))
    {
      autoLightIndex = 0; 
    }
}
#else
void autoFadeAll()
{
    setAll(fadeAllLevel);
    fadeAllLevel += fadeAllDirection;
    if (fadeAllLevel>FULL_INTENSITY)
    {
      fadeAllLevel = FULL_INTENSITY;
      fadeAllDirection = fadeAllDirection * DIR_BK;
    }
    
    if (fadeAllLevel<NO_INTENSITY)
    {
      fadeAllLevel = NO_INTENSITY;
      fadeAllDirection = fadeAllDirection * DIR_BK;
    }
}

void autoFadeOff()
{    
    setAll(ON);
    setOne(autoLightIndex , autoCurrLevel);  

    autoCurrLevel += autoIncr;

    if (autoCurrLevel >= FULL_INTENSITY)
    {
      autoIncr = autoIncr * DIR_BK;
    }

    if (autoCurrLevel <= NO_INTENSITY && autoIncr < 0)
    {
      autoLightIndex ++;
      autoIncr = autoIncr * DIR_BK;
    }
    
    if (autoLightIndex >(connectedLights-1))
    {
      autoLightIndex = 0; 
    }
}
#endif

#endif

int readInt(char arr[],int startidx,int endidx)
{
  int rtn = 0;
  int p = endidx-startidx;
  int idx = startidx;
  for (int i=p;i >= 1;i--)
  {
    // ASCII '0' through '9' characters ar
    // represented by the values 48 through 57.
    // so if the user types a number from 0 through 9 in ASCII, 
    // you can subtract 48 to get the actual value:
    rtn += (arr[idx]-48)*pow(10,i);
    idx ++;
  }
  rtn += arr[endidx]-48;

  return rtn;
}

// This method sends bits to the shift register:
void setOutput(int whichPin, int whichState,int intensity) {
    setOne(whichPin,(whichState == HIGH) ? ON :  OFF);
}

void reset() {
  Serial.println(F("Reset"));
  mSF_Pin = -1;
  setAll(OFF);
}

//Implementations here
#if defined _SHIFTPWM_
void setOne(int whichPin, int level)
{
  ShiftPWM.SetOne(whichPin,level);
}

void setAll(int level)
{
  ShiftPWM.SetAll(level);
}

void setAmountOfRegisters(int numRegisters)
{
  ShiftPWM.SetAmountOfRegisters(numRegisters);  
}

void printLoad()
{
  ShiftPWM.PrintInterruptLoad();    
}

void setupRegisters()
{ 
  pinMode(ShiftPWM_latchPin, OUTPUT);  
  SPI.setBitOrder(LSBFIRST);
  // SPI_CLOCK_DIV2 is only a tiny bit faster in sending out the last byte. 
  // SPI transfer and calculations overlap for the other bytes.
  SPI.setClockDivider(SPI_CLOCK_DIV4); 
  SPI.begin(); 
  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.Start(SPWM_FREQUENCY,FULL_INTENSITY);  
  // Print information about the interrupt frequency, duration and load on your program
  setAll(OFF);
}
#endif


//Implementations here
#if defined _SHIFTOUT_

int getState(int level)
{
  int state = LOW;
  if (level >= 128)
  {
    state = HIGH;
  }
  
  return state;
}

void setOne(int whichPin, int level)
{
  //Shift register only has digital IO
  int state = getState(level);
  int regNo = int(whichPin / lightsPerRegister);
  int actualPin = whichPin - (regNo * lightsPerRegister);
  bitWrite(data[regNo],actualPin,state);
  digitalWrite(latchPin, LOW);
  for (int i=numRegisters-1;i >= 0;i--)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, data[i]);  
      //return the latch pin high to signal chip that it
    //no longer needs to listen for information
  }
  digitalWrite(latchPin, HIGH);
}

void setAll(int level)
{
  int state  = getState(level);
  digitalWrite(latchPin, LOW);
  for (int i=numRegisters-1;i >= 0;i--)
  {
    data[i] = state;
    shiftOut(dataPin, clockPin, MSBFIRST, data[i]);  
      //return the latch pin high to signal chip that it
    //no longer needs to listen for information
  }
  digitalWrite(latchPin, HIGH);
}

void setAmountOfRegisters(int nr)
{
  numRegisters = nr;
}

void printLoad()
{

}

void setupRegisters()
{ 
  pinMode(latchPin, OUTPUT);  
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);  
  setAmountOfRegisters(numRegisters);
  setAll(OFF);
}
#endif

#if defined _TLC5940_
void setOne(int whichPin, int level)
{
  Tlc.set(whichPin,level);
  Tlc.update();
}

void setAll(int level)
{
  Tlc.setAll(level);
  Tlc.update();
}

void setAmountOfRegisters(int nr)
{
  numRegisters=nr;  
}

void printLoad()
{

}

void setupRegisters()
{ 
  Tlc.init();
  // Print information about the interrupt frequency, duration and load on your program
  setAll(OFF);
}
#endif

