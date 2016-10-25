/*
 * interrupt.c
 *
 * This file includes all interrupt handlers.
 *  Created on: Jan 12, 2016
 *      Author: L_Rui
 */

#include "main.h"
#include "gpio.h"
#include <sk4.h>

/* ----------------------------------------------------------------------------
 * Function      : void DMA0_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : Handle the interrupt from DMA channel 0
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : Only complete interrupt is enabled
 * Date			 : 2016-01-20
 * ------------------------------------------------------------------------- */
void DMA0_IRQHandler(void){
	uint32_t	dma_status;
	dma_status = Sys_DMA_Get_Channel0_Status();
	// Clear the status of the DMA channel 0, so that it is ready for the next block
	Sys_DMA_Clear_Channel0_Status();

	if (dma0_wave_left > 0){
		DMA->CH0_SRC_BASE_ADDR = dma0_current_address + dma0_wave_offset;		// Shift the base address of DMA0
		// Compute the length of the DMA session
		if (dma0_wave_left > dma0_wave_length){
			// We have more waiting to be sent!!!
			DMA->CH0_CTRL1 = 0x0000FFFF & dma0_wave_length;
			// Enable Channel 0
			DMA_CH0_CTRL0->ENABLE_ALIAS = 0x1;
			dma0_wave_left -= dma0_wave_length;
		}
		else{
			// We can send all of the lest in this session.
			DMA->CH0_CTRL1 = 0x0000FFFF & dma0_wave_left;
			// Enable Channel 0
			DMA_CH0_CTRL0->ENABLE_ALIAS = 0x1;
			// clear variables
			dma0_wave_length = 0;
			dma0_wave_left = 0;
			dma0_wave_offset = 0;
		}
		return;
	}
	if (dma0_next_state == 1){
		// Change the source address and control 1 register
		DMA->CH0_SRC_BASE_ADDR = dma0_next_address;
		DMA->CH0_CTRL1 = dma0_next_ctrl1;

		dma0_next_state = 0;
		if (dma0_current_address == dma0_src_address_1){
			// The previous one is buffer 1
		    pcm_buffer_1_len = 0;
		    pcm_buffer_1_st = 0;
		}
		else{
			// The previous one is buffer 2
			pcm_buffer_2_len = 0;
			pcm_buffer_2_st = 0;
		}
		dma0_current_address = dma0_next_address;
		dma0_next_address = 0;
		dma0_next_state = 0;
		if ((dma0_next_ctrl1 < 0x00010000)&&(cfx_cmd_state == 0)){
	    	// Notice CFX that the input memory is empty
	    	Sys_CFX_Command(CFX_CMD_0);
	    	cfx_cmd_state = 1;
		}
		dma0_next_ctrl1 = 0;

		// Enable Channel 0
		DMA_CH0_CTRL0->ENABLE_ALIAS = 0x1;
		if (systick_cnt > 0){
			SysTick_Config(systick_cnt);			// Start the Systick counter
		}
	}
	else{
		// Clear the current
		uint32_t tmp;
		if (dma0_current_address == dma0_src_address_1){
			// The previous one is buffer 1
			tmp = pcm_buffer_1[0] >> 16;
			if (tmp == 0x003C){
				DMA_CH0_CTRL0->ENABLE_ALIAS = 0x1;
			}
			else{
				pcm_buffer_1_len = 0;
				pcm_buffer_1_st = 0;
				dma0_current_address = 0;
		    	PCM->TX_DATA = 0x92492490;
			}
		}
		else{
			// The previous one is buffer 2
			tmp = pcm_buffer_2[0] >> 16;
			if (tmp == 0x003C){
				DMA_CH0_CTRL0->ENABLE_ALIAS = 0x1;
			}
			else{
				pcm_buffer_2_len = 0;
				pcm_buffer_2_st = 0;
				dma0_current_address = 0;
		    	PCM->TX_DATA = 0x92492490;
			}
		}
    	// Notice CFX that the input memory is empty
    	if (cfx_cmd_state == 0){
    	   Sys_CFX_Command(CFX_CMD_0);
    	   cfx_cmd_state = 1;
    	}
	}
	return;

}

/* ----------------------------------------------------------------------------
 * Function      : void CFX_0_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : Handle the interrupt from CFX, command 0 (PCM_INPUT is
 * 					full).
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * Date			 : 2016-01-20
 * ------------------------------------------------------------------------- */
void CFX_0_IRQHandler(void){
		// Find the available PCM buffer
		volatile uint8_t*	current_buffer_st;
		volatile uint32_t*	current_buffer_len;

		cfx_cmd_state = 0;
		if (pcm_buffer_1_st == 0){
			// The 1st pcm_buffer is empty
			current_buffer_st = & pcm_buffer_1_st;
			current_buffer_len = & pcm_buffer_1_len;
			PCM_Codec(pcm_buffer_1, current_buffer_st, current_buffer_len);
			// Notify DSP that the 2nd buffer is empty
			//if (pcm_buffer_2_st == 0){
			if ((pcm_buffer_2_st == 0)&&(cfx_cmd_state == 0)){
				Sys_CFX_Command(CFX_CMD_0);
				cfx_cmd_state = 1;
			}
		}
		else if (pcm_buffer_2_st == 0){
			// The 2nd pcm_buffer is empty
			current_buffer_st = & pcm_buffer_2_st;
			current_buffer_len = & pcm_buffer_2_len;
			PCM_Codec(pcm_buffer_2, current_buffer_st, current_buffer_len);
			// Notify DSP that the 1st buffer is empty
			//if (pcm_buffer_1_st == 0){
			if ((pcm_buffer_1_st == 0)&&(cfx_cmd_state == 0)){
				Sys_CFX_Command(CFX_CMD_0);
				cfx_cmd_state = 1;
			}
		}
		else{
			Sys_CFX_Command(CFX_CMD_0);
		}
	return;
}

/* ----------------------------------------------------------------------------
 * Function      : void CFX_1_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : Handle the interrupt from CFX, command 1 (Start GPIO input).
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * Date			 : 2016-01-20
 * ------------------------------------------------------------------------- */
/*void CFX_1_IRQHandler(void){
	Sys_DMA_Clear_Channel0_Status();									// Clear the status of DMA channel 0
	DMA_CH0_CTRL0->ENABLE_ALIAS = DMA_CH0_DISABLE;						// Disable DMA channel 0
    PCM->TX_DATA = 0xFFFFFFFF;											// Make sure S wave is sending

	DIO->INT_CFG_0 = DIO_GPIO_IN_INT;									// Turn on GPIO interrupt
}*/

/* ----------------------------------------------------------------------------
 * Function      : void GPIO_IRQHandler(void)
 * ----------------------------------------------------------------------------
 * Description   : Handle the GPIO input interrupt
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : The interrupt is triggered by the rising edge of the input.
 * Date			 : 2016-01-11
 * ------------------------------------------------------------------------- */
void GPIO_IRQHandler(void){
    Sys_DIO_IntConfig(0, DIO_GPIO_IN_NONE);								// Disable the GPIO input interrupt
	DIO_DATA->ALIAS[GPIO_OUT_SYNC] = 1;									// Debug purpose: Outputting a rising edge at the port "GPIO_OUT_SYNC" indicates the sampling starts.
	systick_cnt = SYSTICK_COUNTDOWN;
	SysTick_Config(systick_cnt);										// Enable the SysTick clock: counter flag not zero, using core clock, enable interrupt, enable timer
    pcm_ret[0] = GPIO_IN_PROCESS;										// Update the return state as "in process"
	// Initialize some local variables
    dio_in_data = 1UL;													// The value at bit 0 is 1
	dio_in_bit = 1;														// The next bit index in dio_data to be read is 1

	//Sys_CM3_Delay(GPIO_DELAY_SYSTICK);  								// Delay: ensure the sampling point is at the middle of the input pulse
	DIO_DATA->ALIAS[GPIO_OUT_SYNC] = 0;									// Debug purpose: Outputting a falling edge at the port "GPIO_OUT_SYNC" indicates the sampling is over.
}

/* ----------------------------------------------------------------------------
 * Function      : void SysTick_Handler(void)
 * ----------------------------------------------------------------------------
 * Description   : Handle the SysTick interrupt
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * Date			 : 2016-01-11
 * ------------------------------------------------------------------------- */
void SysTick_Handler(void){

	DIO_DATA->ALIAS[GPIO_OUT_SYNC] = 1;									// Debug purpose: Outputting a rising edge at the port "GPIO_OUT_SYNC" indicates the sampling starts.

	// Check if it is the backward sampling timer or the backward starting timer
	if (systick_cnt != SYSTICK_COUNTDOWN){
		// Start the GPIO input interrupt
		DIO->INT_CFG_0 = DIO_GPIO_IN_INT;
		SysTick->CTRL = 0;
		return;
	}
	dio_in_data = dio_in_data << 1;										// Left shift the bits in "dio_in_data"
	dio_in_data += DIO_DATA->ALIAS[GPIO_IN_PIN];						// Read the GPIO input and saves it at the bit 0 of "dio_in_data"
	++dio_in_bit;														// Update the index of the next bit

	// Check if we have collect enough bits: >=, yes; <, no
	if (dio_in_bit >= dio_in_length){
		if (dio_in_length == GPIO_LENGTH_SHORT){
			SysTick->CTRL = 0;												// Stop the SysTick
			SysTick->VAL = SYSTICK_COUNTDOWN;								// Reset the SysTick
			pcm_ret[0] = GPIO_COMPLETED;									// Update the status in the pcm return memory
			pcm_ret[1] = dio_in_data;										// Save the final read into the pcm return memory (offset 1)
			if (dio_in_data != dio_in_check){
				DIO_DATA->ALIAS[GPIO_OUT_IND] = 1;
			}
		}
		else{
			// Impedance measurement
			++dio_in_word_cnt;
			pcm_ret[dio_in_word_cnt] = dio_in_data;
			dio_in_data = 0;
			dio_in_bit = 0;
			if (dio_in_word_cnt == 256){
				// We have received all of the data
				pcm_ret[0] = GPIO_COMPLETED;
				dio_in_word_cnt = 0;
			}
		}
		if (cfx_cmd_state == 0){
			Sys_CFX_Command(CFX_CMD_0);
			cfx_cmd_state = 1;
		}
	}
	DIO_DATA->ALIAS[GPIO_OUT_SYNC] = 0;									// Debug purpose: Outputting a falling edge at the port "GPIO_OUT_SYNC" indicates the sampling is over.
	return;
}
