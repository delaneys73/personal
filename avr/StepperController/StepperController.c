/*
 * StepperContoller.c
 *
 *  Created on: 23/06/2012
 *      Author: Stephen Delaney
 */
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>


#define ENABLE_PIN PB3
#define ERROR_PIN PD7
#define STEP_PIN PD2
#define CHG_DIR_PIN PD3

void readSettings();
void doStep(int dir);

//Macros

#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))

//Which step is the motor current on
int step_number=-1;

// How many steps per revolution the motor is capable of
int number_of_steps=20;
int real_number_of_steps=0;
int use_half_step = 1;

//8 steps
int step_count = 8;

volatile int direction=1;

//Motor pins on 9,10,12,13
int half_steps[8] = {0b00011010, //1010
                     0b00011000, //0010
                     0b00011100, //0110
                     0b00001100, //0100
                     0b00101100, //0101
                     0b00101000, //0001
                     0b00101010, //1001
                     0b00001010  //1000
                     };
int full_steps[4] = {
                    0b00011010, //1010
                    0b00011100, //0110
                    0b00101100, //0101
                    0b00101010, //1001
                  };

// This gives us an 8 step buffer if controller is going too fast
volatile int step_register = 0;
volatile int dir_register = 0;
volatile int step_buffer_idx = 0;

void setup()
{
  MCUCR |= (3 << ISC00);    // set INT0 to trigger on ANY logic change
  MCUCR |= (1 << ISC01);    // set INT0 to trigger on ANY logic change
  GICR |= (1 << INT0);      // Turns on INT0
  GICR |= (1 << INT1);      // Turns on INT0

  sei();                    // Enable global interrupts

  //Set 9,10,11,12,13 as output
  int SET_OUPUT = 0b00111110;
  DDRB = DDRB | SET_OUPUT;
  readSettings();

  //attachInterrupt(0,intStep,RISING);
  //attachInterrupt(1,intChgDir,CHANGE);
  DDRD &= ~_BV(CHG_DIR_PIN);
  PORTB |= (1 << ENABLE_PIN);
}

void readSettings()
{
  if (real_number_of_steps==0)
  {
     //Store some logical defaults
     real_number_of_steps = 20;
  }

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
}

/*
* Interrupts
*/
ISR(INT0_vect)
{
	  step_register = step_register << 1;
	  dir_register = dir_register << 1;
	  bit_set(step_register,step_buffer_idx);
	  bit_write(dir_register,step_buffer_idx,direction);
	  step_buffer_idx++;
	  if (step_buffer_idx >7)
	  {
	    step_buffer_idx = 0;
	  }
}

ISR(INT1_vect)
{
	  int newDir = PIND & _BV(CHG_DIR_PIN);
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
	 PORTD |= (1 << ERROR_PIN);
   }
   else
   {
	 PORTD &= ~(1 << ERROR_PIN);
   }

   if (step_register>0)
   {
     doStep(bit_get(dir_register,0));
     step_register = step_register >> 1;
     dir_register = dir_register >> 1;
     step_buffer_idx--;
   }
}

int main(void) {
  setup();

  while (1)
  {
    loop();
  }

  return 0;
}
