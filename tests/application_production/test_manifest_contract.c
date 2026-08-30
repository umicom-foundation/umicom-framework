/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_manifest_contract.c
 *
 * PURPOSE:
 *   Implement the test manifest contract behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | manifest_contract | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionManifestContract contract;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_manifest_contract_build(&binding, &contract) == UMI_STATUS_OK);
    assert(contract.panel_count == binding.experience->panel_count);
    return 0;
}

