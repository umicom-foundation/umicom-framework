/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_page_descriptor.c
 *
 * PURPOSE:
 *   Validate describe a visual application page, route and semantic root component.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/page_descriptor.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadPageDescriptor item;CHECK(umi_rad_page_descriptor_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_page_descriptor_is_valid(&item));return 0;}
