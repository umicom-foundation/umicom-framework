/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_workspace_guide.c
 *
 * PURPOSE:
 *   Verify every application can obtain safe workspace choices and truthful
 *   roadmap guidance from its canonical Framework experience.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/productisation/workspace_guide.h"
#include "umicom/test_runtime/check.h"

/* Exercise the public guide with a representative graphical application. */
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
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find(adoption.application_id);
    UmiProductWorkspaceGuide guide;
    const UmiProductWorkspaceGuideChoice *recommended;

    UMI_TEST_REQUIRE(experience != NULL);
    UMI_TEST_REQUIRE(umi_product_workspace_guide_build(&adoption, &guide) ==
           UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_workspace_guide_validate(&guide) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(strcmp(guide.application_id, adoption.application_id) == 0);
    UMI_TEST_REQUIRE(strcmp(guide.display_name, adoption.display_name) == 0);
    UMI_TEST_REQUIRE(guide.choice_count == experience->layout_count);
    UMI_TEST_REQUIRE(guide.total_panel_placements >= experience->panel_count);
    UMI_TEST_REQUIRE(guide.readiness_percent ==
           umi_application_experience_readiness_percent(experience));
    UMI_TEST_REQUIRE(guide.planned_feature_count + guide.foundation_feature_count +
               guide.implemented_feature_count + guide.verified_feature_count ==
           experience->feature_count);

    recommended = umi_product_workspace_guide_recommended(&guide);
    UMI_TEST_REQUIRE(recommended != NULL);
    UMI_TEST_REQUIRE(recommended->default_layout);
    UMI_TEST_REQUIRE(strcmp(recommended->layout_id,
                            experience->default_layout_id) == 0);
    UMI_TEST_REQUIRE(umi_product_workspace_guide_find(
        &guide, "missing-layout") == NULL);
    UMI_TEST_REQUIRE(umi_product_workspace_guide_at(
        &guide, guide.choice_count) == NULL);
    UMI_TEST_REQUIRE(umi_product_workspace_guide_build(&adoption, NULL) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
