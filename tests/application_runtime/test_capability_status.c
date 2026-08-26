/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_capability_status.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_capability_status(void){ UmiApplicationCapabilityStatus s; assert(umi_application_capability_status_build(test_trader_experience(),test_capability_probe,NULL,&s)==UMI_STATUS_OK); assert(s.entry_count>0U); return 0; }
