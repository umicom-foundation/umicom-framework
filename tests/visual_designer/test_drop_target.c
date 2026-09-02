/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_drop_target.c
 *
 * PURPOSE:
 *   Validate represent validated parent/slot destinations during component drag and drop.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/drop_target.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDropTarget item;CHECK(umi_rad_drop_target_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_drop_target_is_valid(&item));return 0;}
