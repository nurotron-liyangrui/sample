/* ----------------------------------------------------------------------------
 * Copyright (c) 2013 Semiconductor Components Industries, LLC
 * (d/b/a ON Semiconductor). All rights reserved.
 *
 * This code is the property of ON Semiconductor and may not be redistributed
 * in any form without prior written permission from ON Semiconductor. The
 * terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 * ----------------------------------------------------------------------------
 * memory.c
 * - Memory allocation
 * ----------------------------------------------------------------------------
 * $Revision: 1.1 $
 * $Date: 2013/02/26 16:42:21 $
 * ------------------------------------------------------------------------- */

#include "main.h"

/* ----------------------------------------------------------------------------
 * Allocate system stack
 *
 * The variable itself isn't used, but space for it is reserved in the
 * uninitialized .stack section at the top of DRAM. This ensures that the
 * space is available and doesn't conflict with other variables or the heap.
 * It is marked "used" so that link-time optimization doesn't toss it out
 * because it isn't referenced by anything. The linker ensures that the .stack
 * section is 8-byte aligned, so the initial stack pointer is 8-byte aligned.
 * ------------------------------------------------------------------------- */
static
__attribute((used))
__attribute__((section(".stack")))
unsigned char system_stack[STACK_SIZE];
