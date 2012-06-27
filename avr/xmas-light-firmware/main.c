/*
 * main.c
 *
 *  Created on: 25/06/2012
 *      Author: Stephen
 */
#include "main.h"
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr-standard.h>
#include <uart.h>
#include <shiftout-spi.h>
#include <tlc5940.h>

int main() {
	setup();

	while (1){
		loop();
	}

	return 0;
}


void setup()
{
	uart_init(9600);
	shiftout_init();
	eeprom_write_byte(0,99);
	TLC5940_Init();
}

void loop()
{
	TLC5940_SetGS(0,255);
}
