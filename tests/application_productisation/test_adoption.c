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
#include "umicom/test_runtime/check.h"
#include <stdlib.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/productisation/adoption.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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
    const UmiApplicationExperienceDefinition *experience;
    UmiProductApplicationAdoptionSnapshot *snapshot =
        (UmiProductApplicationAdoptionSnapshot *)calloc(1U, sizeof(*snapshot));
    UmiApplicationSuiteLayoutRuntime *layout_runtime =
        (UmiApplicationSuiteLayoutRuntime *)calloc(1U, sizeof(*layout_runtime));

    /* Runtime layout storage expands with reusable window metadata, so the
     * regression fixture uses checked heap storage instead of the test stack. */
    UMI_TEST_REQUIRE(snapshot != NULL && layout_runtime != NULL);
    UMI_TEST_REQUIRE(umi_product_application_adoption_validate(&adoption) ==
           UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_application_adoption_snapshot(
        &adoption, snapshot) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(strcmp(snapshot->application_id,
                            "org.umicom.studio") == 0);
    /* Compare every total with the canonical catalogue. This still catches a
     * projection defect but does not reject deliberate application growth. */
    experience = umi_application_experience_catalogue_find(
        adoption.application_id);
    UMI_TEST_REQUIRE(experience != NULL);
    UMI_TEST_REQUIRE(snapshot->feature_count == experience->feature_count);
    UMI_TEST_REQUIRE(snapshot->panel_count == experience->panel_count);
    UMI_TEST_REQUIRE(snapshot->layout_count == experience->layout_count);
    UMI_TEST_REQUIRE(snapshot->projected_layout_count == snapshot->layout_count);
    UMI_TEST_REQUIRE(snapshot->projected_window_count >= snapshot->panel_count);
    UMI_TEST_REQUIRE(snapshot->default_layout_window_count > 0U);
    UMI_TEST_REQUIRE(snapshot->layout_runtime_ready);
    UMI_TEST_REQUIRE(snapshot->layout_projection_complete);
    UMI_TEST_REQUIRE(snapshot->covered_surface_count == snapshot->panel_count);
    UMI_TEST_REQUIRE(snapshot->missing_surface_count == 0U);
    UMI_TEST_REQUIRE(snapshot->runnable);
    UMI_TEST_REQUIRE(umi_product_application_adoption_layout_load(
        &adoption, layout_runtime) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(layout_runtime->loaded);
    UMI_TEST_REQUIRE(layout_runtime->experience != NULL);
    UMI_TEST_REQUIRE(
        umi_product_application_adoption_snapshot_accepted(snapshot));
    free(layout_runtime);
    free(snapshot);
    return 0;
}
