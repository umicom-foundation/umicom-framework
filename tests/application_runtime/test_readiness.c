/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_readiness.c
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
/*
 * Exercise test readiness and return a clear result when the behaviour no longer matches
 * its contract.
 */
int test_readiness(void){ UmiApplicationReadinessReport r; assert(umi_application_readiness_report(test_trader_experience(),&r)==UMI_STATUS_OK); assert(r.feature_count>0U); assert(r.readiness_percent<=100U); return 0; }
