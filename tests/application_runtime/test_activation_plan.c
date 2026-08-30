/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_runtime/test_activation_plan.c
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
int test_activation_plan(void){ UmiApplicationActivationPlan p; assert(umi_application_activation_plan_build(test_trader_experience(),NULL,test_capability_probe,NULL,&p)==UMI_STATUS_OK); assert(p.entry_count>0U); return 0; }
