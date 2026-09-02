/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_flex_constraint.c
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
#include "umicom/designer/visual_designer/flex_constraint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadFlexConstraint item;CHECK(umi_rad_flex_constraint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_flex_constraint_is_valid(&item));return 0;}
