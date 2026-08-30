/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_layout_constraint.c
 *
 * PURPOSE:
 *   Validate describe minimum/maximum geometry constraints for designer components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/layout_constraint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadLayoutConstraint item;CHECK(umi_rad_layout_constraint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_layout_constraint_is_valid(&item));return 0;}
