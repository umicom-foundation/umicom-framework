/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_adoption.c
 *
 * PURPOSE:
 *   Verify thin product contributions obtain readiness and surface evidence
 *   exclusively from canonical Framework catalogues.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/application/productisation/adoption.h"

int main(void)
{
    const UmiProductApplicationAdoption adoption = {
        sizeof(UmiProductApplicationAdoption),
        "studio",
        "org.umicom.studio",
        "Umicom Studio IDE",
        "umicom-studio-ide",
        UMI_PRODUCT_FRONTEND_FLAG_CONSOLE | UMI_PRODUCT_FRONTEND_FLAG_GTK4,
        1, 1, 1, 1
    };
    UmiProductApplicationAdoptionSnapshot snapshot;
    UmiApplicationSuiteLayoutRuntime layout_runtime;
    assert(umi_product_application_adoption_validate(&adoption) ==
           UMI_STATUS_OK);
    assert(umi_product_application_adoption_snapshot(
        &adoption, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.application_id, "org.umicom.studio") == 0);
    assert(snapshot.feature_count == 9U);
    assert(snapshot.panel_count == 12U);
    assert(snapshot.layout_count == 3U);
    assert(snapshot.projected_layout_count == snapshot.layout_count);
    assert(snapshot.projected_window_count >= snapshot.panel_count);
    assert(snapshot.default_layout_window_count > 0U);
    assert(snapshot.layout_runtime_ready);
    assert(snapshot.layout_projection_complete);
    assert(snapshot.covered_surface_count == snapshot.panel_count);
    assert(snapshot.missing_surface_count == 0U);
    assert(snapshot.runnable);
    assert(umi_product_application_adoption_layout_load(
        &adoption, &layout_runtime) == UMI_STATUS_OK);
    assert(layout_runtime.loaded);
    assert(layout_runtime.experience != NULL);
    assert(umi_product_application_adoption_snapshot_accepted(&snapshot));
    return 0;
}
