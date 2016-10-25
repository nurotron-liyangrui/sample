/* ----------------------------------------------------------------------------
 * Copyright (c) 2013 Semiconductor Components Industries, LLC
 * (d/b/a ON Semiconductor). All rights reserved.
 *
 * This code is the property of ON Semiconductor and may not be redistributed
 * in any form without prior written permission from ON Semiconductor. The
 * terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 * ----------------------------------------------------------------------------
 * initialize.c
 *  - Initialization source file
 * ----------------------------------------------------------------------------
 * $Revision: 1.15 $
 * $Date: 2013/02/27 20:52:12 $
 * ------------------------------------------------------------------------- */

#include "main.h"

/* ----------------------------------------------------------------------------
 * Function      : void Var_init(void)
 * ----------------------------------------------------------------------------
 * Description   : Performs the system variable initialization
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void Var_init(void){
	pcm_buffer_1_st = 0;					// The 1st PCM buffer is empty
	pcm_buffer_2_st = 0;					// The 2nd PCM buffer is empty
	pcm_buffer_1_len = 0;					// The data length in pcm_1 is 0
	pcm_buffer_2_len = 0;					// The data length in pcm_2 is 0
}


/* ----------------------------------------------------------------------------
 * Function      : void Initialize(void)
 * ----------------------------------------------------------------------------
 * Description   : Performs all system initialization tasks.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void Initialize(void)
{
    /* Disable exceptions (except NMI and the hard fault exception) and
     * interrupts before configuring interfaces and peripherals by setting a
     * 1 to the 1-bit interrupt mask register PRIMASK */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);

    /* Clear the enable for all of the external interrupts supported by the
     * NVIC (Nested Vectored Interrupt Controller). */
    Sys_NVIC_DisableAllInt();

    /* Clear the pending status for all of the external interrupts. */
    Sys_NVIC_ClearAllPendingInt();

    // Set the priorities of the interrupts
    SCB->AIRCR = (NVIC_VECTKEY_UNLOCK | \
   		 	 	NVIC_LITTLE_ENDIAN | \
   		 	 	NVIC_PRIGROUP_2_2);
    // NVIC_PRIGROUP_2_2: 7-6 bits priority group; 5-4 bits subgroup. Lower number -> higher priority
    NVIC_IP2->CFX_0_PRIORITY_BYTE = 0xC0;			// CFX priority group 3; subgroup 0
    NVIC_IP4->DMA0_PRIORITY_BYTE = 0x50;			// DMA priority group 1; subgroup 1
    NVIC_IP5->GPIO_PRIORITY_BYTE = 0x40;			// GPIO priority group 1; subgroup 0
    //SCB_SHP2->NVIC_SYSTICK_PRIORITY_BYTE = 0x01;	// Systick priority group 0; subgroup 1


    /* Disable all DMAs */
    Sys_DMA_Channel0_Disable();
    Sys_DMA_Channel1_Disable();
    Sys_DMA_Channel2_Disable();
    Sys_DMA_Channel3_Disable();

    /* Clear the DMA status */
    Sys_DMA_Clear_Channel0_Status();
    Sys_DMA_Clear_Channel1_Status();
    Sys_DMA_Clear_Channel2_Status();
    Sys_DMA_Clear_Channel3_Status();

    /* Disable the PCM */
     Sys_PCM_Disable();
     /* Configure the PCM */
     Sys_PCM_Config(PCM_CFG);
     /* Enable the PCM */
     Sys_PCM_Enable();

     /* Disable the ACB */
     ACB->CTRL = ACB_CTRL_DISABLE_VAL;

     /* Configure the DMAs */
     Sys_DMA_Channel0_Config(DMA_CH0_CFG_VAL,     				      \
     						 DMA_CH0_TRANSFER_LENGTH,    			  \
                             DMA_CH0_COUNTER_INT_NONE,				  \
                             dma0_src_address_1,         			  \
                             DMA_CH0_DEST_ADDRESS);





    /* Enable individual external interrupts */
    NVIC_EnableIRQ(CFX_0_IRQn);
    NVIC_EnableIRQ(DMA0_IRQn);
    NVIC_EnableIRQ(GPIO_IRQn);


    // Initialize the system variables
    Var_init();

    /* Un-mask exceptions and interrupts by setting a 0 to the 1-bit interrupt
     * mask register PRIMASK */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);

}
