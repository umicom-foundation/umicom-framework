/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_capability_requirement.c
 *
 * PURPOSE:
 *   Implement the test capability requirement behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | capability_requirement | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionCapabilityRequirements requirements;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_capability_requirements_build(&binding, &requirements) == UMI_STATUS_OK);
    assert(requirements.count > 0U);
    assert(requirements.count <= binding.experience->panel_count + binding.experience->feature_count);
    return 0;
}

