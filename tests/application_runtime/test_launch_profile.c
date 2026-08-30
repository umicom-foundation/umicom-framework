/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_launch_profile.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
#include "umicom/runtime/application_manifest.h"
int test_launch_profile(void){ UmiApplicationLaunchProfile p; assert(umi_application_launch_profile_default(test_trader_experience(),UMI_FRONTEND_CONSOLE,&p)==UMI_STATUS_OK); assert(umi_application_launch_profile_validate(test_trader_experience(),&p)==UMI_STATUS_OK); return 0; }
