/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_device_profile.c
 *
 * PURPOSE:
 *   Validate describe preview device dimensions, density and input characteristics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/device_profile.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadDeviceProfile item;CHECK(umi_rad_device_profile_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_device_profile_is_valid(&item));return 0;}
