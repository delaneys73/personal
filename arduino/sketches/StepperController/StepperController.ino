#include <EEPROM.h>

//Which step is the motor current on
int step_number=-1;

// How many steps per revolution the morot is capable of
int number_of_steps=0;
int real_number_of_steps=0;


int use_half_step = 1;


//8 steps
int step_count = 8;

const int CHG_DIR_PIN=3;
const int ERROR_PIN=7;
volatile int direction=1;

//EEPROM constants
const int ADDR_STEPS=0;
const int ADDR_USE_HALF=1;

//Motor pins on 9,10,12,13
byte half_steps[] = {B00011010, //1010
                     B00011000, //0010
                     B00011100, //0110
                     B00001100, //0100
                     B00101100, //0101
                     B00101000, //0001
                     B00101010, //1001
                     B00001010  //1000
                  };
byte full_steps[] = {
                    B00011010, //1010
                    B00011100, //0110
                    B00101100, //0101
                    B00101010, //1001
                  };

// This gives us an 8 step buffer if controller is going too fast
volatile byte step_register = 0;
volatile byte dir_register = 0;
volatile int step_buffer_idx = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Stepper Controller V1");
  //Set 9,10,11,12,13 as output
  DDRB = DDRB | B00111110;
  readSettings(); 
  
  attachInterrupt(0,intStep,RISING);
  attachInterrupt(1,intChgDir,CHANGE);
  digitalWrite(11,HIGH);
}

void readSettings()
{
  real_number_of_steps = EEPROM.read(ADDR_STEPS);
  if (real_number_of_steps==0)
  {
     //Store some logical defaults 
     Serial.println("Writing defaults to EEPROM");
    
     real_number_of_steps = 20;
     EEPROM.write(ADDR_STEPS,real_number_of_steps);
     EEPROM.write(ADDR_USE_HALF,use_half_step);
  }

  real_number_of_steps = EEPROM.read(ADDR_STEPS);
  use_half_step = EEPROM.read(ADDR_USE_HALF);
  number_of_steps = real_number_of_steps;
  if (use_half_step)
  {
     number_of_steps *= 2; 
     step_count = 8;     
  }
  else
  {
     step_count = 4; 
  }
     
  Serial.print("steps: ");
  Serial.println(number_of_steps);
  Serial.print("mode: ");
  Serial.println(use_half_step);
}

/*
* Interrupts
*/
void intStep()
{
  step_register = step_register << 1;
  dir_register = dir_register << 1;  
  bitSet(step_register,step_buffer_idx);
  bitWrite(dir_register,step_buffer_idx,direction);
  step_buffer_idx++;
  if (step_buffer_idx >7)
  {
    step_buffer_idx = 0;
  }
}

void intChgDir()
{
  int newDir = digitalRead(CHG_DIR_PIN);
  direction = newDir;  
}

/*
  Moves the motor steps_to_move steps.  If the number is negative, 
   the motor moves in the reverse direction.
 */
void doStep(int dir)
{  
        // increment or decrement the step number,
        // depending on direction:
        if (dir == 0) 
        {
          step_number++;
          if (step_number >= number_of_steps) 
          {
            step_number = 0;
          }
        } 
        else 
        { 
          if (step_number < 0) 
          {
            step_number = number_of_steps;
          }
          step_number--;
        }
        // step the motor to step number 0, 1, 2, or 3:
        int thisStep = (step_number % step_count);
        /*
         * Moves the motor forward or backwards.
         * On pins 9,10,11,12
         */        
        #if defined(DEBUG)
        Serial.print("SI:");
        Serial.println(thisStep);
        #endif
        if (use_half_step==0)
        {
            PORTB = full_steps[thisStep];
        }
        else
        { 
            PORTB = half_steps[thisStep];
        } 
}

void loop()
{
   if (step_register==255)
   {
     digitalWrite(ERROR_PIN,HIGH);
   }
   else
   {
     digitalWrite(ERROR_PIN,LOW);
   }
   
   if (step_register>0)
   {
     #if defined(DEBUG)
     Serial.print("STEP:");
     Serial.println(step_register);
     #endif
     doStep(bitRead(dir_register,0));
     step_register = step_register >> 1;
     dir_register = dir_register >> 1;
     step_buffer_idx--;
   }
   
   if (Serial.available())
   {
       char command[1];
       Serial.readBytesUntil(',',command,sizeof(command));
       char value[4];
       Serial.readBytesUntil('\n',value,sizeof(value));
       processCommand(command,value);
   }
}

void processCommand(char command[1],char value[4])
{
    int val = 0;
    int pows[] = {1000,100,10,1};
    
    for (int x=0;x < 4;x++)
    {
      val += (value[x]-48)*pows[x];
    }

    int addr = -1;

    if (command[0]=='S')
    {
       addr = ADDR_STEPS;
    }
    
    if (command[0]=='H')
    {
       addr = ADDR_USE_HALF;
    }
    
    if (addr!=-1)
    {
       EEPROM.write(addr,val);
    }
    
    readSettings();
}
