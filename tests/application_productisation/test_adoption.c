/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_adoption.c
 *
 * PURPOSE:
 *   Verify thin product contributions obtain readiness and surface evidence
 *   exclusively from canonical Framework catalogues.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
        UMI_PRODUCT_FRONTEND_CONSOLE | UMI_PRODUCT_FRONTEND_GTK4,
        1, 1, 1, 1
    };
    UmiProductApplicationAdoptionSnapshot snapshot;
    assert(umi_product_application_adoption_validate(&adoption) ==
           UMI_STATUS_OK);
    assert(umi_product_application_adoption_snapshot(
        &adoption, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.application_id, "org.umicom.studio") == 0);
    assert(snapshot.feature_count == 9U);
    assert(snapshot.panel_count == 12U);
    assert(snapshot.layout_count == 3U);
    assert(snapshot.covered_surface_count == snapshot.panel_count);
    assert(snapshot.missing_surface_count == 0U);
    assert(snapshot.runnable);
    assert(umi_product_application_adoption_snapshot_accepted(&snapshot));
    return 0;
}
