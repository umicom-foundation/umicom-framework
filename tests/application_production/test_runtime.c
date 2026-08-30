/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_runtime.c
 *
 * PURPOSE:
 *   Implement the test runtime behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | runtime | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionRuntime *runtime = calloc(1U, sizeof(*runtime));
    assert(runtime != NULL);
    assert(umi_application_production_runtime_init(&adoption, umi_test_application_production_all_available, NULL, runtime) == UMI_STATUS_OK);
    assert(runtime->panels.count > 0U && runtime->layouts.count > 0U);
    assert(umi_application_production_runtime_checkpoint(runtime, "test", 1) == UMI_STATUS_OK);
    free(runtime);
    return 0;
}

