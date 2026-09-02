/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_guide.c
 *
 * PURPOSE:
 *   Validate represent user-created horizontal and vertical design guides.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/guide.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadGuide item;CHECK(umi_rad_guide_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_guide_is_valid(&item));return 0;}
