/*
 * variable.c
 *
 * This file declare the global variables
 *  Created on: Jan 12, 2016
 *      Author: L_Rui
 */

#include "main.h"
#include <sk4.h>


/* These parameters are for the PCM RF coding */
volatile uint32_t		pcm_buffer_1[PCM_FIFO_SIZE];											// Memory: PCM RF coding buffer #1
volatile uint32_t		pcm_buffer_2[PCM_FIFO_SIZE];											// Memory: PCM RF coding buffer #2
volatile uint8_t		pcm_buffer_1_st;														// State: 1, buffer is full; 0, buffer is empty
volatile uint8_t		pcm_buffer_2_st;														// State: 1, buffer is full; 0, buffer is empty
volatile uint32_t		pcm_buffer_1_len;														// Length of data in pcm_buffer_1
volatile uint32_t		pcm_buffer_2_len;														// Length of data in pcm_buffer_2

volatile int			check_16_32;															// Check whether the whole 32 bits have been filled. 1: half filled.
/* These parameters are for the PCM input */
volatile uint32_t * pcm_input = (uint32_t *) (DRAM_TOP + 1 - STACK_SIZE - 4*PCM_INPUT_SIZE - 4);					// The base address of the PCM input memory
volatile uint32_t * pcm_ret = (uint32_t *) (DRAM_TOP + 1 - STACK_SIZE - 4*PCM_INPUT_SIZE - 4*GPIO_RET_SIZE-4);		// The base address of the PCM return memory

/* These parameters are for the DMA */
volatile uint32_t		dma0_current_address;													// The current DMA source address
volatile uint32_t		dma0_next_address;														// The next DMA source address
volatile uint32_t		dma0_next_ctrl1;														// The control word of the next DMA source buffer
volatile uint32_t		dma0_next_state;														// The state of the next DMA source buffer, full 1 or empty 0
uint32_t		dma0_src_address_1 = (uint32_t) pcm_buffer_1;									// The source address of DMA when sending data/cmd
uint32_t		dma0_src_address_2 = (uint32_t) pcm_buffer_2;									// The source address of DMA when sending padding

// These two are for the case that the padding waves are too long to write into one pcm buffer.
volatile uint32_t		dma0_wave_offset;														// The starting offset of the wave in the pcm buffer
volatile uint32_t		dma0_wave_left = 0;														// The length of data words left to be sent
volatile uint32_t		dma0_wave_length;														// The length of the wave in the pcm buffer

/* These parameters are for the GPIO input */
int				dio_in_bit = 0;															// The current bit index of GPIO input series
uint32_t		dio_in_data = 0;														// The up-to-date GPIO input data
uint32_t		dio_in_check;															// The expected GPIO input data
uint32_t		dio_in_length;															// The expected GPIO input data length
uint32_t		dio_in_word_cnt = 0;													// The count of received words
volatile int				cfx_cmd_state = 0;


/* These variables are the SysTick timer */
volatile 	uint32_t			systick_cnt;													// The load value of the Systick timer

