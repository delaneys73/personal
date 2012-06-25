/*
 * main.c
 *
 *  Created on: 25/06/2012
 *      Author: Stephen
 */
#include "main.h"
#include <avr/io.h>
#include <uart.h>

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
}

void loop()
{

}
