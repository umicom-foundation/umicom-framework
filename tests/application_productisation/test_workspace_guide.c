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
#include <assert.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/productisation/workspace_guide.h"

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

    assert(experience != NULL);
    assert(umi_product_workspace_guide_build(&adoption, &guide) ==
           UMI_STATUS_OK);
    assert(umi_product_workspace_guide_validate(&guide) == UMI_STATUS_OK);
    assert(strcmp(guide.application_id, adoption.application_id) == 0);
    assert(strcmp(guide.display_name, adoption.display_name) == 0);
    assert(guide.choice_count == experience->layout_count);
    assert(guide.total_panel_placements >= experience->panel_count);
    assert(guide.readiness_percent ==
           umi_application_experience_readiness_percent(experience));
    assert(guide.planned_feature_count + guide.foundation_feature_count +
               guide.implemented_feature_count + guide.verified_feature_count ==
           experience->feature_count);

    recommended = umi_product_workspace_guide_recommended(&guide);
    assert(recommended != NULL);
    assert(recommended->default_layout);
    assert(strcmp(recommended->layout_id, experience->default_layout_id) == 0);
    assert(umi_product_workspace_guide_find(&guide, "missing-layout") == NULL);
    assert(umi_product_workspace_guide_at(&guide, guide.choice_count) == NULL);
    assert(umi_product_workspace_guide_build(&adoption, NULL) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
