/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_feature_binding.c
 *
 * PURPOSE:
 *   Implement the test feature binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | feature_binding | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionFeatureBindings features;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_feature_bindings_build(&binding, &features) == UMI_STATUS_OK);
    assert(features.count == binding.experience->feature_count);
    assert(features.open_count <= features.count);
    return 0;
}

