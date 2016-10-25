/* ----------------------------------------------------------------------------
 * Copyright (c) 2013 Semiconductor Components Industries, LLC
 * (d/b/a ON Semiconductor). All rights reserved.
 *
 * This code is the property of ON Semiconductor and may not be redistributed
 * in any form without prior written permission from ON Semiconductor. The
 * terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 * ----------------------------------------------------------------------------
 * main.c
 * - Main source file
 * ----------------------------------------------------------------------------
 * $Revision: 1.31 $
 * $Date: 2013/04/12 19:06:18 $
 * ------------------------------------------------------------------------- */

#include "main.h"


/* ----------------------------------------------------------------------------
 * Function      : int main(void)
 * ----------------------------------------------------------------------------
 * Description   : Initializes the system, and starts an infinite while loop.
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
int main(void)
{
    /* Call the system initialization function */
    Initialize();


    while (1)
    {
    }

    return 0;   /* Not reached; main() loops forever */
}
