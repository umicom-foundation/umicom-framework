/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_manifest_snapshot.c
 *
 * PURPOSE:
 *   Implement the test manifest snapshot behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | manifest_snapshot | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionManifestSnapshot snapshot;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_manifest_snapshot_capture(&binding, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.manifest_available && snapshot.tests_available);
    return 0;
}

