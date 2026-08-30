/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_diagnostic_report.c
 *
 * PURPOSE:
 *   Implement the test diagnostic report behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | diagnostic_report | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionRuntime *runtime = calloc(1U, sizeof(*runtime));
    UmiApplicationProductionDiagnosticReport report;
    assert(runtime != NULL);
    assert(umi_application_production_runtime_init(&adoption, umi_test_application_production_all_available, NULL, runtime) == UMI_STATUS_OK);
    assert(umi_application_production_diagnostic_report_build(runtime, &report) == UMI_STATUS_OK);
    assert(report.count >= 1U);
    free(runtime);
    return 0;
}

