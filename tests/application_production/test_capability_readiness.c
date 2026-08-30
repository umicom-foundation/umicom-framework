/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_capability_readiness.c
 *
 * PURPOSE:
 *   Implement the test capability readiness behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | capability_readiness | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "test_fixture.h"
int main(void) {
    UmiApplicationProductionCapabilityProbeResults results;
    UmiApplicationProductionCapabilityRequirement requirement = {{0}, 1U, 0U, 1U};
    UmiApplicationProductionCapabilityReadiness readiness;
    (void)strcpy(requirement.capability_id, "test.capability");
    (void)memset(&results, 0, sizeof(results));
    results.entries[0].requirement = &requirement;
    results.count = 1U;
    results.unavailable_count = 1U;
    results.critical_unavailable_count = 1U;
    assert(umi_application_production_capability_readiness_evaluate(&results, &readiness) == UMI_STATUS_OK);
    assert(readiness.state == UMI_APPLICATION_PRODUCTION_BLOCKED);
    assert(!umi_application_production_capability_readiness_launchable(&readiness));
    return 0;
}

