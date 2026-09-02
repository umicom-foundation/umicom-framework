/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_layout_binding.c
 *
 * PURPOSE:
 *   Implement the test layout binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | layout_binding | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionLayoutBindings layouts;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_layout_bindings_build(&binding, &layouts) == UMI_STATUS_OK);
    assert(layouts.count == binding.experience->layout_count);
    assert(layouts.projected_count == layouts.count);
    assert(umi_application_production_layout_bindings_find(&layouts, binding.experience->default_layout_id) != NULL);
    return 0;
}

