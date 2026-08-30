/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_manifest_drift.c
 *
 * PURPOSE:
 *   Implement the test manifest drift behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | manifest_drift | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionManifestContract expected;
    UmiApplicationProductionManifestSnapshot actual;
    UmiApplicationProductionManifestDrift drift;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_manifest_contract_build(&binding, &expected) == UMI_STATUS_OK);
    assert(umi_application_production_manifest_snapshot_capture(&binding, &actual) == UMI_STATUS_OK);
    actual.observed.panel_count += 1U;
    assert(umi_application_production_manifest_drift_compare(&expected, &actual, &drift) == UMI_STATUS_OK);
    assert(!drift.compatible);
    assert((drift.flags & UMI_APPLICATION_PRODUCTION_DRIFT_PANELS) != 0U);
    return 0;
}

