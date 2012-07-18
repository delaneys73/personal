/*
 * StepperContoller.c
 *
 *  Created on: 23/06/2012
 *      Author: Stephen Delaney
 */
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr-standard.h>
#include <uart.h>


#define ENABLE_PIN PB3
#define ERROR_PIN PD7
#define STEP_PIN PD2
#define CHG_DIR_PIN PD3

void readSettings();
void doStep(int dir);
void writeSettings();

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

//Interrupt stuff
int const INT_RISING  = 3;
int const INT_FALLING = 2;
int const INT_CHANGE = 1;

int const ADDR_STEPS = 51;
int const ADDR_HALFSTEP = 52;

void setup()
{
  MCUCR |= (INT_RISING << ISC00);    // set INT0 to trigger on RISING logic change
  MCUCR |= (INT_CHANGE << ISC01);    // set INT1 to trigger on ANY logic change
  GICR |= (1 << INT0);      // Turns on INT0
  GICR |= (1 << INT1);      // Turns on INT0

  sei();                    // Enable global interrupts

  //Set 9,10,11,12,13 as output
  int SET_OUPUT = 0b00111110;
  DDRB = DDRB | SET_OUPUT;

  readSettings();

  set_output(DDRD,CHG_DIR_PIN);
  output_high(PORTB,ENABLE_PIN);

  uart_init(9600);
  uart_puts("Stepper Controller v1\n");
}

void readSettings()
{
  real_number_of_steps = eeprom_read_byte((uint8_t*)ADDR_STEPS);

  if (real_number_of_steps==0)
  {
     //Store some logical defaults
     real_number_of_steps = 20;
     use_half_step = 1;

     eeprom_write_byte((uint8_t*)ADDR_STEPS,real_number_of_steps);
     eeprom_write_byte((uint8_t*)ADDR_HALFSTEP,use_half_step);
  }
  else
  {
	  use_half_step = eeprom_read_byte((uint8_t*)ADDR_HALFSTEP);
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
	  int newDir = get_value(PIND,CHG_DIR_PIN);
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
	 output_high(PORTD,ERROR_PIN);
   }
   else
   {
	 output_low(PORTD,ERROR_PIN);
   }

   if (step_register>0)
   {
     doStep(bit_get(dir_register,0));
     step_register = step_register >> 1;
     dir_register = dir_register >> 1;
     step_buffer_idx--;
   }

   if (uart_available()>0)
   {
	   writeSettings();
   }
}

void writeSettings()
{

}

int main(void) {
  setup();

  while (1)
  {
    loop();
  }

  return 0;
}
