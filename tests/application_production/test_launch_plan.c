/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_launch_plan.c
 *
 * PURPOSE:
 *   Implement the test launch plan behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | launch_plan | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "test_fixture.h"
int main(void) {
    UmiApplicationProductionLaunchPlan plan;
    UmiApplicationProductionLifecycleGate gate;
    int value;
    assert(umi_application_production_launch_plan_build(&plan) == UMI_STATUS_OK);
    for (value = UMI_APPLICATION_PRODUCTION_GATE_CONTRACT; value <= UMI_APPLICATION_PRODUCTION_GATE_ACCEPTANCE; ++value) {
        (void)memset(&gate, 0, sizeof(gate));
        gate.gate = (UmiApplicationProductionGate)value;
        gate.passed = 1;
        assert(umi_application_production_launch_plan_apply_gate(&plan, &gate) == UMI_STATUS_OK);
    }
    assert(umi_application_production_launch_plan_ready(&plan));
    return 0;
}

