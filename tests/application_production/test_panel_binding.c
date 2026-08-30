/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_panel_binding.c
 *
 * PURPOSE:
 *   Implement the test panel binding behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | panel_binding | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionPanelBindings panels;
    UmiProductSurfacePortfolio *surfaces = calloc(1U, sizeof(*surfaces));
    assert(surfaces != NULL);
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_product_surface_portfolio_build(surfaces) == UMI_STATUS_OK);
    assert(umi_application_production_panel_bindings_build(&binding, surfaces, &panels) == UMI_STATUS_OK);
    assert(panels.count == binding.experience->panel_count);
    assert(panels.uncovered_count == 0U);
    free(surfaces);
    return 0;
}

