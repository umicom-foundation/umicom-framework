/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_evidence_requirement.c
 *
 * PURPOSE:
 *   Implement the test evidence requirement behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | evidence_requirement | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionEvidenceRequirements requirements;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_evidence_requirements_build(&binding, &requirements) == UMI_STATUS_OK);
    assert(requirements.required_count > 2U);
    assert(requirements.count >= requirements.required_count);
    return 0;
}

