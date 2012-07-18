/*
 * avr-standard.c
 *
 *  Created on: 26/06/2012
 *      Author: Stephen
 */

#include "avr-standard.h"

//General IO
// Some macros that make the code more readable
void output_low(int port,int pin) {port &= ~(1<<pin);}
void output_high(int port,int pin) {port |= (1<<pin);}
void set_input(int portdir,int pin) {portdir &= ~(1<<pin);}
void set_output(int portdir,int pin) {portdir |= (1<<pin);}
int get_value(int port, int pin) {return ((port) & (1 << (pin)));}
void pulse(int port, int pin){
	do {
		output_high((port), (pin));
		output_low((port), (pin));
	} while (0);
}
void pulse_pin(int port,int pin){
	port |= _BV(pin);
	port &= ~_BV(pin);
}

// this is just a program that 'kills time' in a calibrated method
void delay_ms(uint8_t ms) {
	uint16_t delay_count = F_CPU / 17500;
	volatile uint16_t i;

	while (ms != 0) {
		for (i=0; i != delay_count; i++);
		 ms--;
	}
}

//Bit manipulation
int bit_get(int p,int m){ return ((p) & (m));}
void bit_set(int p,int m){ ((p) |= (m));}
void bit_clear(int p,int m){ ((p) &= ~(m));}
void bit_flip(int p,int m){ ((p) ^= (m));}
void bit_write(int c,int p,int m) {(c ? bit_set(p,m) : bit_clear(p,m));}
int BIT(int x) {return (0x01 << (x));}
int LONGBIT(int x) {return ((unsigned long)0x00000001 << (x));}
