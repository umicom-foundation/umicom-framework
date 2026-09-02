/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_snap_result.c
 *
 * PURPOSE:
 *   Validate record the deterministic outcome of a snap calculation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/snap_result.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadSnapResult item;CHECK(umi_rad_snap_result_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_snap_result_is_valid(&item));return 0;}
