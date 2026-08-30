/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_runtime_health.c
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
int test_runtime_health(void){ UmiApplicationRuntimeHealth h; assert(umi_application_runtime_health_evaluate(test_trader_experience(),test_capability_probe,NULL,&h)==UMI_STATUS_OK); assert(h.readiness_percent<=100U); return 0; }
