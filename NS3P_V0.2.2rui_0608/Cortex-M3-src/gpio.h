/*
 * gpio.h
 *
 *  Created on: Mar 14, 2016
 *      Author: L_Rui
 */

#ifndef GPIO_H_
#define GPIO_H_

// State of returns
#define GPIO_IN_PROCESS					0
#define GPIO_COMPLETED					1
#define GPIO_FAILED						2

// The length of the GPIO input word
#define GPIO_LENGTH_SHORT				16
#define GPIO_LENGTH_LONG				32

// The table of time delays to start GPIO input
#define GPIO_DELAY_SYS_ON				94
#define	GPIO_DELAY_LEVEL				94
#define	GPIO_DELAY_CMD					65
#define GPIO_DELAY_IMPEDANCE			65

// The time delay between the first GPIO interrupt and
// the time to start SYSTICK
#define	GPIO_DELAY_SYSTICK				15


#endif /* GPIO_H_ */
