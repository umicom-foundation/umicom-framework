/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_flex_constraint.c
 *
 * PURPOSE:
 *   Validate describe renderer-neutral flexible-box growth and basis constraints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/flex_constraint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadFlexConstraint item;CHECK(umi_rad_flex_constraint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_flex_constraint_is_valid(&item));return 0;}
