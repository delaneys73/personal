/*
 * blink.c
 *
 *  Created on: 25/06/2012
 *      Author: Stephen
 */

#include "blink.h"

// this is the header file that tells the compiler what pins and ports, etc.
// are available on this chip.
#include <avr/io.h>
#include <avr-standard.h>
// define what pins the LEDs are connected to.
// in reality, PD6 is really just '6'
#define LED PD6

void setup()
{
	// initialize the direction of PORTD #6 to be an output
	set_output(DDRD, LED);
}


void loop()
{
	// turn on the LED for 200ms
	 output_high(PORTD, LED);
	 delay_ms(200);
	 // now turn off the LED for another 200ms
	 output_low(PORTD, LED);
	 delay_ms(200);
	 // now start over
}

int main(void) {
	setup();

	while (1) {
		 loop();
	}
}
