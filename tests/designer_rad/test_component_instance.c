/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_component_instance.c
 *
 * PURPOSE:
 *   Validate represent one semantic component instance on a designer document.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/component_instance.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadComponentInstance item;CHECK(umi_rad_component_instance_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_component_instance_is_valid(&item));return 0;}
