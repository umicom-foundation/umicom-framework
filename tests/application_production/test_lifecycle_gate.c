/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_lifecycle_gate.c
 *
 * PURPOSE:
 *   Implement the test lifecycle gate behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | lifecycle_gate | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionRuntime *runtime = calloc(1U, sizeof(*runtime));
    UmiApplicationProductionLifecycleGate gate;
    assert(runtime != NULL);
    assert(umi_application_production_runtime_init(&adoption, umi_test_application_production_all_available, NULL, runtime) == UMI_STATUS_OK);
    assert(umi_application_production_lifecycle_gate_evaluate(UMI_APPLICATION_PRODUCTION_GATE_ACCEPTANCE, &runtime->binding, &runtime->readiness, &runtime->manifest_drift, &runtime->recovery, &runtime->acceptance, &gate) == UMI_STATUS_OK);
    assert(gate.passed);
    free(runtime);
    return 0;
}

