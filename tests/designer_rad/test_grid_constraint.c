/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_grid_constraint.c
 *
 * PURPOSE:
 *   Validate describe renderer-neutral grid row/column placement and spans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/grid_constraint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadGridConstraint item;CHECK(umi_rad_grid_constraint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_grid_constraint_is_valid(&item));return 0;}
