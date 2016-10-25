/* ----------------------------------------------------------------------------
 * Copyright (c) 2013 Semiconductor Components Industries, LLC
 * (d/b/a ON Semiconductor). All rights reserved.
 *
 * This code is the property of ON Semiconductor and may not be redistributed
 * in any form without prior written permission from ON Semiconductor. The
 * terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 * ----------------------------------------------------------------------------
 * main.h
 * - Main application-level include file
 * ----------------------------------------------------------------------------
 * $Revision: 1.35 $
 * $Date: 2013/04/12 19:06:18 $
 * ------------------------------------------------------------------------- */

#ifndef MAIN_H
#define MAIN_H

#include <sk4.h>


/* ----------------------------------------------------------------------------
 * Application configuration
 * ------------------------------------------------------------------------- */

/* Size of the stack in bytes */
#define STACK_SIZE                      0x400

/* Size of the PCM FIFO: PCM transmission rate is 3.2MHz, PCM register length
 * is 32 bit, and the maximum segment period is 8 ms. So, the number of words is
 * 800. Pick a number which is slightly greater than what we need.
 * */
#define	PCM_FIFO_SIZE					410

/* Size of the PCM input: 256. It must be consistent to the definition in DSP */
#define PCM_INPUT_SIZE					256

/* Size of the Back return: 257. It must be consistent to the definition in DSP */
#define GPIO_RET_SIZE					257

/* ----------------------------------------------------------------------------
 * ACB configuration
 * ------------------------------------------------------------------------- */
/* ACB control setting used to disable the codecs during hardware
 * initialization */
#define ACB_CTRL_DISABLE_VAL            (G722_DEC_DISABLE               | \
                                         G722_ENC_DISABLE               | \
                                         CVSD_DEC_DISABLE               | \
                                         CVSD_ENC_DISABLE)

/* ----------------------------------------------------------------------------
 * PCM configuration
 * ------------------------------------------------------------------------- */
#define PCM_CFG							(C_PCM_BIT_ORDER_MSB_FIRST		| \
										 C_PCM_TX_ALIGN_LSB				| \
										 C_PCM_WORD_SIZE_32				| \
										 C_PCM_FRAME_ALIGN_LAST			| \
										 C_PCM_FRAME_WIDTH_SHORT		| \
										 C_PCM_MULTIWORD_2				| \
										 C_PCM_SUBFRAME_DISABLE			| \
										 C_PCM_CONTROLLER_DMA			| \
										 C_PCM_DISABLE					| \
										 C_PCM_SELECT_MASTER)
/* ----------------------------------------------------------------------------
 * GPIO configuration
 * ------------------------------------------------------------------------- */
/* The pin indices must match the definition in the DSP core */
#define	GPIO_IN_PIN						8
#define GPIO_OUT_IND					21
#define	GPIO_OUT_SYNC					16

/* ----------------------------------------------------------------------------
 * SysTick configuration
 * The main clock rate is 6.4Mhz and the symbol rate is 50k bits/s. So the
 * sampling interval is 128
 * ------------------------------------------------------------------------- */

#define SYSTICK_COUNTDOWN             	128

/* ----------------------------------------------------------------------------
 * PCM padding patterns
 * ------------------------------------------------------------------------- */
#define NULL_WAVE						0x00000000
#define ZERO_WAVE_1						0x92492492
#define ZERO_WAVE_2						0x49249249
#define ZERO_WAVE_3						0x24924924
#define ONE_WAVE_1						0xDB6DB6DB
#define	ONE_WAVE_2						0x6DB6DB6D
#define ONE_WAVE_3						0xB6DB6DB6
#define	POWER_WAVE						0xAAAAAAAA
#define S_WAVE							0xFFFFFFFF

// ----------------------------------------------------------------------------
//  PCM input types
// ----------------------------------------------------------------------------
#define CODED_DATA_TYPE             1
#define UNCODED_DATA_TYPE           2
#define NULL_WAVE_TYPE              4
#define ZERO_WAVE_TYPE              8
#define ONE_WAVE_TYPE               16
#define POWER_WAVE_TYPE             32
#define S_WAVE_TYPE                 64
#define END_PCM_INPUT               0xFF

// The types below are something special about the command type
#define COMMAND_LEVEL_SHIFT			3
#define COMMAND_GENERAL				5
#define COMMAND_SYS_ON				7
#define COMMAND_IMPEDANCE			9

/* DMA 0 configuration M-P (Memory to Peripheral), where the memory is the
 * RF coded data, and the peripheral is PCM output
 * Byte ordering is little endian.
 * Source word size is 32 bits and destination word sizes is 32 bits.
 * The source address increments by a step size of 1 and the destination
 * address is static. Addressing mode of the transfer is set to be linear. In
 * this mode, each time this DMA channel is enabled, it transfers one buffer of
 * data of size DMA_CH0_TRANSFER_LENGTH.
 * The DMA 0 complete interrupt is enabled. */
#define DMA_CH0_CFG_VAL				    (DMA_CH0_DEST_ADDR_STEP_SIZE_1  | \
                                         DMA_CH0_SRC_ADDR_STEP_SIZE_1   | \
                                         DMA_CH0_DEST_ADDR_POS          | \
                                         DMA_CH0_SRC_ADDR_POS           | \
                                         DMA_CH0_LITTLE_ENDIAN          | \
                                         DMA_CH0_DISABLE_INT_DISABLE    | \
                                         DMA_CH0_ERROR_INT_DISABLE      | \
                                         DMA_CH0_COMPLETE_INT_ENABLE   	| \
                                         DMA_CH0_COUNTER_INT_ENABLE     | \
                                         DMA_CH0_START_INT_DISABLE      | \
                                         DMA_CH0_DEST_WORD_SIZE_32      | \
                                         DMA_CH0_SRC_WORD_SIZE_32       | \
                                         DMA_CH0_TRANSFER_M_TO_P        | \
                                         DMA_CH0_DEST_ADDR_STATIC       | \
                                         DMA_CH0_DEST_PCM            	| \
                                         DMA_CH0_SRC_ADDR_INC           | \
                                         DMA_CH0_PRIORITY_0             | \
                                         DMA_CH0_ADDR_LIN               | \
                                         DMA_CH0_DISABLE)


#define DMA_CH0_DEST_ADDRESS            (uint32_t) (&PCM->TX_DATA)
#define	DMA_CH0_TRANSFER_LENGTH			PCM_FIFO_SIZE
/* Since DMA channel 0 counter interrupt is disabled, the counter interrupt
 * value does not matter and is left at the default value 0 */
#define DMA_CH0_COUNTER_INT_NONE          0
#define DMA_COUNTER_INT_STATUS			  0x00000020
#define DMA_COMPLETE_INT_STATUS			  0x00000040

// Configuration of the GPIO input interrupt
#define	DIO_GPIO_IN_INT					0x00000068
#define	DIO_GPIO_IN_NONE				0x00000008





/* ----------------------------------------------------------------------------
 * Prototypes for global variables
 * ------------------------------------------------------------------------- */
volatile extern uint32_t	pcm_buffer_1[PCM_FIFO_SIZE];					// Memory: PCM RF coding buffer #1
volatile extern uint32_t	pcm_buffer_2[PCM_FIFO_SIZE];					// Memory: PCM RF coding buffer #2
extern volatile uint8_t		pcm_buffer_1_st;								// State: 1, buffer is full; 0, buffer is empty
extern volatile uint8_t		pcm_buffer_2_st;								// State: 1, buffer is full; 0, buffer is empty
extern volatile uint32_t	pcm_buffer_1_len;								// Length of data in pcm_buffer_1
extern volatile uint32_t	pcm_buffer_2_len;								// Length of data in pcm_buffer_2
extern volatile uint32_t	pcm_buffer_1_cnt;								// Counter value to trigger the counter interrupt of buffer 1
extern volatile uint32_t	pcm_buffer_2_cnt;								// Counter value to trigger the counter interrupt of buffer 2
extern volatile int			check_16_32;									// Check whether the whole 32 bits have been filled. 1: half filled.

extern volatile uint32_t * 	pcm_input;										// The base address of the PCM input memory
extern volatile uint32_t *  pcm_ret;										// The base address of the PCM return memory

extern volatile uint32_t	dma0_current_address;							// The current DMA source address
extern volatile uint32_t	dma0_next_address;								// The next DMA source address
extern volatile uint32_t	dma0_next_ctrl1;								// The length of current DMA source buffer
extern volatile uint32_t	dma0_next_state;								// The state of the next DMA source buffer, full 1 or empty 0

extern uint32_t				dma0_src_address_1;								// The 1st candidate DMA source address
extern uint32_t				dma0_src_address_2;								// The 2nd candidate DMA source address

extern volatile uint32_t	dma0_wave_offset;								// The starting offset of the wave in the pcm buffer
extern volatile uint32_t	dma0_wave_left;									// The length of data words left to be sent
extern volatile uint32_t	dma0_wave_length;								// The length of the wave in the pcm buffer


extern int					dio_in_bit;										// The current bit index of GPIO input series
extern uint32_t				dio_in_data;									// The up-to-date GPIO input data
extern uint32_t				dio_in_check;									// The expected GPIO input data
extern uint32_t				dio_in_length;									// The expected GPIO input data bit-length
extern uint32_t				dio_in_word_cnt;								// The count of received words
extern int					receive_state;

extern volatile int					cfx_cmd_state;

extern volatile uint32_t	systick_cnt;									// The load value of the Systick timer



/* ----------------------------------------------------------------------------
 * Prototypes for global functions
 * ------------------------------------------------------------------------- */
extern void Initialize(void);
extern void PCM_Codec(volatile uint32_t* , volatile uint8_t* , volatile uint32_t*);
extern void CFX_0_IRQHandler(void);
extern void GPIO_IRQHandler(void);
extern void SysTick_Handler(void);

#endif /* MAIN_H */
