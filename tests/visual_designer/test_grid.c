/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_grid.c
 *
 * PURPOSE:
 *   Validate describe configurable design-time grid spacing and visibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/grid.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDesignGrid item;CHECK(umi_rad_grid_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_grid_is_valid(&item));return 0;}
