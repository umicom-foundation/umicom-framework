/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_snap_policy.c
 *
 * PURPOSE:
 *   Validate configure grid, guide and component snapping tolerance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/snap_policy.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadSnapPolicy item;CHECK(umi_rad_snap_policy_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_snap_policy_is_valid(&item));return 0;}
