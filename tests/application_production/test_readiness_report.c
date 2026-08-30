/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_readiness_report.c
 *
 * PURPOSE:
 *   Implement the test readiness report behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | readiness_report | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionCapabilityRequirements requirements;
    UmiApplicationProductionCapabilityProbeResults results;
    UmiApplicationProductionCapabilityReadiness capabilities;
    UmiApplicationProductionReadinessReport report;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_capability_requirements_build(&binding, &requirements) == UMI_STATUS_OK);
    assert(umi_application_production_capability_probe_run(&requirements, umi_test_application_production_all_available, NULL, &results) == UMI_STATUS_OK);
    assert(umi_application_production_capability_readiness_evaluate(&results, &capabilities) == UMI_STATUS_OK);
    assert(umi_application_production_readiness_report_build(&binding, &capabilities, &report) == UMI_STATUS_OK);
    assert(report.combined_percent <= 100U);
    assert(report.layout_ready);
    return 0;
}

