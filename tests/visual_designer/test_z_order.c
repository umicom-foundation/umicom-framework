/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_z_order.c
 *
 * PURPOSE:
 *   Validate represent component stacking order independently of renderer implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/z_order.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadZOrder item;CHECK(umi_rad_z_order_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_z_order_is_valid(&item));return 0;}
