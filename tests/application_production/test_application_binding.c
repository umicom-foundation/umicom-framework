/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_application_binding.c
 *
 * PURPOSE:
 *   Implement the test application binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | application_binding | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_binding_validate(&binding) == UMI_STATUS_OK);
    assert(binding.adoption_snapshot.layout_projection_complete);
    return 0;
}

