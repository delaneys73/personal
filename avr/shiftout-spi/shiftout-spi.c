#include <avr/io.h>

#define SHIFT_REGISTER DDRB
#define SHIFT_PORT PORTB
#define DATA (1<<PB3)     //MOSI (SI)
#define LATCH (1<<PB2)        //SS   (RCK)
#define CLOCK (1<<PB5)        //SCK  (SCK)

void shiftout_init()
{
	//Setup IO
	SHIFT_REGISTER |= (DATA | LATCH | CLOCK); //Set control pins as outputs
	SHIFT_PORT &= ~(DATA | LATCH | CLOCK);        //Set control pins low

	//Setup SPI
	SPCR = (1<<SPE) | (1<<MSTR);  //Start SPI as Master
}

void shiftout_shift(int byte)
{
	//Shift in some data
	SPDR = byte;        //This should light alternating LEDs
	//Wait for SPI process to finish
	while(!(SPSR & (1<<SPIF)));
}

void shiftout_start()
{
	//Pull LATCH low (Important: this is necessary to start the SPI transfer!)
	SHIFT_PORT &= ~LATCH;
}

void shiftout_end()
{
	//Toggle latch to copy data to the storage register
	SHIFT_PORT |= LATCH;
	SHIFT_PORT &= ~LATCH;
}
