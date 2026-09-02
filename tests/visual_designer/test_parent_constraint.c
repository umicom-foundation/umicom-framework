/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_parent_constraint.c
 *
 * PURPOSE:
 *   Validate describe which semantic component families a parent slot accepts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/parent_constraint.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadParentConstraint item;CHECK(umi_rad_parent_constraint_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_parent_constraint_is_valid(&item));return 0;}
