/*
 * shiftout-spi.h
 *
 *  Created on: 25/06/2012
 *      Author: Stephen
 */

#ifndef SHIFTOUT_SPI_H_
#define SHIFTOUT_SPI_H_

void shiftout_init();
void shiftout_shift(int byte);
void shiftout_start();
void shiftout_end();

#endif /* SHIFTOUT_SPI_H_ */
