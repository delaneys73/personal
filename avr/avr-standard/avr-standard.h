/*
 * avr-standard.h
 *
 *  Created on: 26/06/2012
 *      Author: Stephen
 */
#include <avr/io.h>

#ifndef AVR_STANDARD_H_
#define AVR_STANDARD_H_

//General IO
void output_low(int port,int pin);
void output_high(int port,int pin);
void set_input(int portdir,int pin);
void set_output(int portdir,int pin);
int get_value(int port, int pin);
void pulse(int port, int pin);
void pulse_pin(int port, int pin);

void delay_ms(uint8_t ms);

#endif /* AVR_STANDARD_H_ */
