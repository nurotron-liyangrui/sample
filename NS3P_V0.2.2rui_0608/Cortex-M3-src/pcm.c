/*
 * pcm.c
 *  This file contains functions to perform PCM coding
 *  Created on: Nov 12, 2015
 *      Author: L_Rui
 */
#include "main.h"
#include "table.h"
#include "gpio.h"

/* ------------------------------------------------------------------------------------------------------------------------------------------
 * Function      :  void ADD_Waves(uint32_t* base_addr, uint16_t* len_ptr, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t data_len)
 * -----------------------------------------------------------------------------------------------------------------------------------------
 * Description   : Add the waves (NULL, ZERO, ONE, POWER, S) into the assigned pcm_buffer memory
 * Inputs        : - base_addr				- The base address of the assigned pcm_buffer memory
 * 				   - len_ptr				- The pointer pointing to the current length of the assigned pcm_buffer_memory
 * 				   - data1					- The first data circularly written into pcm_buffer memory
 * 				   - data2					- The second data circularly written into pcm_buffer memory
 * 				   - data3					- The third data circularly written into pcm_buffer memory
 * 				   - data_len				- The number of data inserted into pcm_buffer
 *
 * Outputs       : None
 * Assumptions   : None
 * Date			 : 2016-01-21
 * --------------------------------------------------------------------------------------------------------------- */
void ADD_Waves(volatile uint32_t* base_addr, volatile uint32_t* len_ptr, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t data_len){
	// Get the current length of the pcm_buffer
	uint16_t len = *len_ptr;
	// Circularly insert the three data
	uint32_t i;
	uint32_t modd = data_len%3;

	if (check_16_32 == 1){
		// The current is half full
		i = base_addr[len];
		base_addr[len++] = i | 0x0000FFFF;
		check_16_32 = 0;
	}
	i = 0;
	while ((len < PCM_FIFO_SIZE - 2)&&(i < data_len - 2)){
		base_addr[len++] = data1;
		base_addr[len++] = data2;
		base_addr[len++] = data3;
		i += 3;
	}
	if ((len >= PCM_FIFO_SIZE - 2)&&(i < data_len - 2)){
		// The PCM buffer is full, but there is still "waves" to be sent.
		dma0_wave_offset = len - i;				// The starting offset of the wave in the pcm buffer
		dma0_wave_length = i;					// The length of the wave in the pcm buffer
		dma0_wave_left = data_len - modd - i;	// The length of data words left to be sent

	}
	// Note that we maybe do not fill enough patterns, due to "modd". We are afraid of the inbalance of duty cycle.
	// Update the length of the assigned pcm_buffer
	*len_ptr = len;
	return;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------
 * Function      :  uint16_t ADD_CODED_Data(uint32_t* base_addr, uint16_t* len_ptr, uint16_t input_off, uint32_t data_len)
 * -----------------------------------------------------------------------------------------------------------------------------------------
 * Description   : Add the coded data from the pcm input memory into the pcm buffer memory. Note the data in pcm input memory
 * 				   are in 16 bits (the first 16 bits in one word are 0), but the ones in pcm buffer memory are in 32 bits.
 * Inputs        : - base_addr				- The base address of the assigned pcm_buffer memory
 * 				   - len_ptr				- The pointer pointing to the current length of the assigned pcm_buffer_memory
 * 				   - input_off				- The address offset of the pcm input memory
 * 				   - data_len				- The number of data inserted into pcm_buffer
 * Outputs       : The current address offset of the pcm input memory
 * Assumptions   : None
 * Date			 : 2016-01-21
 * --------------------------------------------------------------------------------------------------------------- */
uint16_t ADD_CODED_Data(volatile uint32_t* base_addr, volatile uint32_t* len_ptr, uint16_t input_off, uint32_t data_len){
	// Get the current length of the pcm_buffer
	uint16_t len = *len_ptr;
	uint32_t i;															// temporary index
	uint32_t input_data_tmp1;											// temporary variable for the pcm input data
	uint32_t input_data_tmp2;											// temporary variable for the pcm input data

	input_data_tmp1 = pcm_input[input_off++];							// Get the first input data word
	if (check_16_32 == 1){												// The current word is already half full
		input_data_tmp2 = base_addr[len];
		base_addr[len++] = (input_data_tmp2 << 16) | input_data_tmp1;	// Store data into pcm buffer memory
		check_16_32 = 0;
		data_len--;
	}
	else{
		input_off--;
	}
	if (data_len % 2 == 0){
		for (i = 0; i < data_len; i +=2){
			input_data_tmp1 = pcm_input[input_off++] << 16;			// Read two words from pcm input memory
			input_data_tmp2 = pcm_input[input_off++];
			base_addr[len++] = input_data_tmp1 | input_data_tmp2;	// Store data into pcm buffer memory
		}
	}
	else{
		for (i = 0; i < (data_len-1); i +=2){
			input_data_tmp1 = pcm_input[input_off++] << 16;			// Read two words from pcm input memory
			input_data_tmp2 = pcm_input[input_off++];
			base_addr[len++] = input_data_tmp1 | input_data_tmp2;	// Store data into pcm buffer memory
		}
		input_data_tmp1 = pcm_input[input_off++];
		base_addr[len] = input_data_tmp1 << 16;						// Store the last data word
		check_16_32 = 1;
	}

	// Update the length of the assigned pcm_buffer
	*len_ptr = len;
	return input_off;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------
 * Function      :  uint16_t PERFROM_Coding(uint32_t* base_addr, uint16_t* len_ptr, uint16_t input_off, uint32_t data_len)
 * -----------------------------------------------------------------------------------------------------------------------------------------
 * Description   : Perform RF coding and save the results into the pcm_buffer memory
 * Inputs        : - base_addr				- The base address of the assigned pcm_buffer memory
 * 				   - len_ptr				- The pointer pointing to the current length of the assigned pcm_buffer_memory
 * 				   - input_off				- The address offset of the pcm input memory
 * 				   - data_len				- The number of data inserted into pcm_buffer
 *
 * Outputs       : The current address offset of the pcm input memory
 * Assumptions   : None
 * Date			 : 2016-01-21
 * --------------------------------------------------------------------------------------------------------------- */
uint16_t PERFROM_Coding(volatile uint32_t* base_addr, volatile uint32_t* len_ptr, uint16_t input_off, uint32_t data_len){
	// Get the current length of the pcm_buffer
	uint16_t len = *len_ptr;
	uint32_t i = 0;												// temporary index
	uint32_t input_data_tmp;									// temporary variable for input data
	uint32_t pcm_data_tmp;										// temporary variable for output data
	uint32_t tmp;												// temporary variable

	if (check_16_32 == 1){
		// The current word is half full
		input_data_tmp = pcm_input[input_off++];						// Read the next 16 bit word to be coded
		pcm_data_tmp = pcm_table[input_data_tmp>>12] << 4;				// Coding the 1st four bits
		pcm_data_tmp |= pcm_table[(input_data_tmp & 0x0F00)>>8] >> 8;	// Coding the 2nd four bits
		base_addr[len++] |= pcm_data_tmp;
		pcm_data_tmp = pcm_table[(input_data_tmp & 0x0F00)>>8] << 24;
		pcm_data_tmp |= pcm_table[(input_data_tmp & 0x00F0)>>4] << 12;	// Coding the 3rd four bits
		pcm_data_tmp |= pcm_table[(input_data_tmp & 0x000F)];			// Coding the 4th four bits
		base_addr[len++] = pcm_data_tmp;
		data_len--;
		check_16_32 = 0;
	}
	if (data_len % 2 == 0){
		for (i = 0; i < data_len; i+= 2){									// We code the input data, 2 16 bits input data => 3 32 bits PCM codes
			input_data_tmp = pcm_input[input_off++];						// Read the 1st input data
			pcm_data_tmp = pcm_table[input_data_tmp>>12]<<20;				// Coding the 1st four bits
			pcm_data_tmp |= pcm_table[(input_data_tmp & 0x0F00)>>8]<<8; 	// Coding the 2nd four bits
			tmp = pcm_table[(input_data_tmp & 0x00F0)>>4];					// Coding the 3rd four bits
			pcm_data_tmp |= tmp>>4;
			base_addr[len++] = pcm_data_tmp;								// Update the assigned pcm_buffer
			pcm_data_tmp = tmp<<28;
			pcm_data_tmp |= pcm_table[(input_data_tmp & 0x000F)]<<16;		// Coding the 4th four bits
			input_data_tmp = pcm_input[input_off++];						// The 2nd input data
			pcm_data_tmp |= pcm_table[input_data_tmp>>12]<<4;				// The 1st four bits
			tmp = pcm_table[(input_data_tmp & 0x0F00)>>8];					// The 2nd four bits
			pcm_data_tmp |= tmp>>8;
			base_addr[len++] = pcm_data_tmp;
			pcm_data_tmp = tmp<<24;
			pcm_data_tmp |= pcm_table[(input_data_tmp & 0x00F0)>>4]<<12;	// The 3rd four bits
			pcm_data_tmp |= pcm_table[(input_data_tmp & 0x000F)];			// The 4th four bits
			base_addr[len++] = pcm_data_tmp;
		}
	}
	else{
		for (i = 0; i < data_len-1; i+= 2){									// We code the input data, 2 16 bits input data => 3 32 bits PCM codes
			input_data_tmp = pcm_input[input_off++];						// Read the 1st input data
			pcm_data_tmp = pcm_table[input_data_tmp>>12]<<20;				// Coding the 1st four bits
			pcm_data_tmp |= pcm_table[(input_data_tmp & 0x0F00)>>8]<<8; 	// Coding the 2nd four bits
			tmp = pcm_table[(input_data_tmp & 0x00F0)>>4];					// Coding the 3rd four bits
			pcm_data_tmp |= tmp>>4;
			base_addr[len++] = pcm_data_tmp;								// Update the assigned pcm_buffer
			pcm_data_tmp = tmp<<28;
			pcm_data_tmp |= pcm_table[(input_data_tmp & 0x000F)]<<16;		// Coding the 4th four bits
			input_data_tmp = pcm_input[input_off++];						// The 2nd input data
			pcm_data_tmp |= pcm_table[input_data_tmp>>12]<<4;				// The 1st four bits
			tmp = pcm_table[(input_data_tmp & 0x0F00)>>8];					// The 2nd four bits
			pcm_data_tmp |= tmp>>8;
			base_addr[len++] = pcm_data_tmp;
			pcm_data_tmp = tmp<<24;
			pcm_data_tmp |= pcm_table[(input_data_tmp & 0x00F0)>>4]<<12;	// The 3rd four bits
			pcm_data_tmp |= pcm_table[(input_data_tmp & 0x000F)];			// The 4th four bits
			base_addr[len++] = pcm_data_tmp;
		}
		input_data_tmp = pcm_input[input_off++];
		pcm_data_tmp = pcm_table[input_data_tmp>>12]<<20;					// Coding the 1st four bits
		pcm_data_tmp |= pcm_table[(input_data_tmp & 0x0F00)>>8]<<8; 		// Coding the 2nd four bits
		tmp = pcm_table[(input_data_tmp & 0x00F0)>>4];						// Coding the 3rd four bits
		pcm_data_tmp |= tmp>>4;
		base_addr[len++] = pcm_data_tmp;									// Update the assigned pcm_buffer
		pcm_data_tmp = tmp<<28;
		pcm_data_tmp |= pcm_table[(input_data_tmp & 0x000F)]<<16;			// Coding the 4th four bits
		base_addr[len] = pcm_data_tmp;
		check_16_32 = 1;
	}
	// Update the length of the assigned pcm_buffer
	*len_ptr = len;
	return input_off;
}

/* ------------------------------------------------------------------------------------------------------------------------------------------
 * Function      :  void PCM_Codec(uint32_t* base_addr, volatile uint8_t* state, volatile uint32_t* len_ptr)
 * -----------------------------------------------------------------------------------------------------------------------------------------
 * Description   : RF coding main function
 * Inputs        : - base_addr				- The base address of the assigned pcm_buffer memory
 * 				   - state					- The pointer pointing to the state of the assigned pcm buffer memory
 * 				   - len_ptr				- The pointer pointing to the current length of the assigned pcm_buffer_memory
 * Outputs       : None
 * Assumptions   : None
 * Date			 : 2016-03-15
 * --------------------------------------------------------------------------------------------------------------- */
void PCM_Codec(volatile uint32_t* base_addr, volatile uint8_t* state, volatile uint32_t* len_ptr){
	uint32_t			frame_var;										// Include the following three frame parameters
	uint32_t			type;											// Frame type
	uint32_t  			par1;											// 1st frame property parameter
	uint32_t			par2;											// 2nd frame property parameter
	uint32_t			input_addr_offset = 0;							// The current address offset of pcm_input memory
	int16_t				cmd_type = -1;
	uint32_t 			tmp;
	check_16_32 = 0;

	while (input_addr_offset < PCM_INPUT_SIZE){
		// Read the frame property
		frame_var = pcm_input[input_addr_offset++];
		type = (frame_var & 0xFF000000)>>24;
		par1 = (frame_var & 0x00FFFF00)>>8;
		par2 = frame_var & 0x000000FF;
		uint32_t wave_len;
		wave_len = par1*par2;

		// Frame is *_Waves; Call ADD_Waves correspondingly
		switch (type){
		case NULL_WAVE_TYPE:										// Add NULL wave
			ADD_Waves(base_addr, len_ptr, NULL_WAVE, NULL_WAVE, NULL_WAVE, wave_len);
			break;
		case ZERO_WAVE_TYPE:										// Add ZERO wave
			ADD_Waves(base_addr, len_ptr, ZERO_WAVE_1, ZERO_WAVE_2, ZERO_WAVE_3, wave_len);
			break;
		case ONE_WAVE_TYPE:											// Add ONE wave
			ADD_Waves(base_addr, len_ptr, ONE_WAVE_1, ONE_WAVE_2, ONE_WAVE_3, wave_len);
			break;
		case POWER_WAVE_TYPE:										// Add POWER wave
			ADD_Waves(base_addr, len_ptr, POWER_WAVE, POWER_WAVE, POWER_WAVE, wave_len);
			break;
		case S_WAVE_TYPE:											// Add S wave
			ADD_Waves(base_addr, len_ptr, S_WAVE, S_WAVE, S_WAVE, wave_len);

			// S wave! We need to open the backward input
			if (cmd_type == COMMAND_SYS_ON){
				// There is no command ahead of this s wave.
				systick_cnt = GPIO_DELAY_SYS_ON;
				dio_in_check = 0xCCCC;
				dio_in_length = GPIO_LENGTH_SHORT;
			}
			else if (cmd_type == COMMAND_LEVEL_SHIFT){
				// There is level command ahead of this s wave
				systick_cnt = GPIO_DELAY_LEVEL;
				dio_in_check = 0xAAAA;
				dio_in_length = GPIO_LENGTH_SHORT;
			}
			else if (cmd_type == COMMAND_GENERAL){
				// There is general command ahead of this s wave
				systick_cnt = GPIO_DELAY_CMD;
				dio_in_check = 0xC000;
				dio_in_length = GPIO_LENGTH_SHORT;
			}
			else if (cmd_type == COMMAND_IMPEDANCE){
				// There is an impedance measurement command ahead of this s wave
				systick_cnt = GPIO_DELAY_IMPEDANCE;
				dio_in_check = 0x0000;
				dio_in_length = GPIO_LENGTH_LONG;
				dio_in_word_cnt = 0;
			}
			else{
				systick_cnt = 0;
			}

			break;
		case UNCODED_DATA_TYPE:
			input_addr_offset = PERFROM_Coding(base_addr, len_ptr, input_addr_offset, par1);
			break;
		case CODED_DATA_TYPE:
			input_addr_offset = ADD_CODED_Data(base_addr, len_ptr, input_addr_offset, par1);
			break;
		case COMMAND_LEVEL_SHIFT:
			cmd_type = COMMAND_LEVEL_SHIFT;
			break;
		case COMMAND_GENERAL:
			cmd_type = COMMAND_GENERAL;
			break;
		case COMMAND_SYS_ON:
			cmd_type = COMMAND_SYS_ON;
			break;
		case COMMAND_IMPEDANCE:
			cmd_type = COMMAND_IMPEDANCE;
			break;
		case END_PCM_INPUT:

			// 04-26-2016: Check if this block is full of stimulations
			tmp = base_addr[0]>>16;
			uint32_t length = *len_ptr;
			if (check_16_32 == 1){
				check_16_32 = 0;
				base_addr[length] = base_addr[length]|0x00009249;
				length++;
				*len_ptr = length;
			}
			// The end of the pcm input memory
			input_addr_offset = PCM_INPUT_SIZE;
			break;
		default:
			return;
		}
	}

	// The whole task is over. Set the state of the assigned pcm buffer. If DMA channel 0 is off, turn it on.
	if ((pcm_buffer_1_st || pcm_buffer_2_st)==0){
		*state = 1;
		// The DMA channel 0 is disabled. Turn it on.
		// Clear the status of the DMA channel 0, so that it is ready for the next block
		Sys_DMA_Clear_Channel0_Status();
		// Set the current DMA source address
		dma0_current_address = (uint32_t) base_addr;

	    DMA->CH0_SRC_BASE_ADDR = dma0_current_address;								// set the source address
		DMA->CH0_CTRL1 = 0x0000FFFF&(*len_ptr);		// update the Transfer length of DMA channel 0
		// Enable the DMA
		DMA_CH0_CTRL0->ENABLE_ALIAS = 0x1;
		if (systick_cnt > 0){
			SysTick_Config(systick_cnt);			// Start the Systick counter
		}

	}
	else{
		*state = 1;
		// One is working, the other one is idle
		dma0_next_address = (uint32_t) base_addr;
		dma0_next_ctrl1 = 0x0000FFFF&(*len_ptr);		// update the Transfer length of DMA channel 0
		dma0_next_state = 1;
	}

	return;
}





