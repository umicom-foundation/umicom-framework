/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_grid.c
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
#include "umicom/designer/rad/grid.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadDesignGrid item;CHECK(umi_rad_grid_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_grid_is_valid(&item));return 0;}
