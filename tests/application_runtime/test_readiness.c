/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_readiness.c
 *
 * PURPOSE:
 *   Exercise one focused application runtime or projection contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int test_readiness(void){ UmiApplicationReadinessReport r; assert(umi_application_readiness_report(test_trader_experience(),&r)==UMI_STATUS_OK); assert(r.feature_count>0U); assert(r.readiness_percent<=100U); return 0; }
