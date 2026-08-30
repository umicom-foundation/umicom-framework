/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_capability_probe.c
 *
 * PURPOSE:
 *   Implement the test capability probe behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | capability_probe | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionCapabilityRequirements requirements;
    UmiApplicationProductionCapabilityProbeResults results;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_capability_requirements_build(&binding, &requirements) == UMI_STATUS_OK);
    assert(umi_application_production_capability_probe_run(&requirements, umi_test_application_production_all_available, NULL, &results) == UMI_STATUS_OK);
    assert(results.available_count == results.count);
    return 0;
}

