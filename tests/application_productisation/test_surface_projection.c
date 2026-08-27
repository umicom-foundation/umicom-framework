/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_surface_projection.c
 *
 * PURPOSE:
 *   Verify every declared product panel resolves to a reusable Framework
 *   component and retains its canonical layout/context metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/application/productisation/surface_projection.h"

int main(void)
{
    UmiProductSurfacePortfolio portfolio;
    const UmiProductSurfaceProjection *editor;
    const UmiProductApplicationSurfaceSummary *studio;
    assert(umi_product_surface_portfolio_build(&portfolio) == UMI_STATUS_OK);
    assert(portfolio.application_count == 25U);
    assert(portfolio.surface_count == 263U);
    assert(portfolio.covered_count == portfolio.surface_count);
    assert(portfolio.missing_count == 0U);
    editor = umi_product_surface_portfolio_find(
        &portfolio, "org.umicom.studio", "editor");
    assert(editor != NULL);
    assert(editor->coverage == UMI_PRODUCT_SURFACE_COVERED);
    assert(strcmp(editor->component_id, "umicom.shared.editor") == 0);
    assert(strcmp(editor->default_region, "centre") == 0);
    studio = umi_product_surface_portfolio_application_find(
        &portfolio, "org.umicom.studio");
    assert(studio != NULL && studio->surface_count == 12U);
    assert(studio->missing_count == 0U);
    assert(umi_application_component_capability_count("umicom.audit") > 0U);
    assert(umi_application_component_capability_at("umicom.audit", 0U) != NULL);
    return 0;
}
