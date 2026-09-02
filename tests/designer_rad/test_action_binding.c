/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_action_binding.c
 *
 * PURPOSE:
 *   Validate bind a designer action surface to a Framework command and target.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/action_binding.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadActionBinding item;CHECK(umi_rad_action_binding_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_action_binding_is_valid(&item));return 0;}
