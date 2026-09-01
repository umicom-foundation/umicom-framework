/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_profile_projection.c
 *
 * PURPOSE:
 *   Exercise Framework-owned Application Suite layout materialisation and coverage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/suite_layout.h"
#include "umicom/test_runtime/check.h"

/* Verify canonical projection succeeds and a same-named foreign layout cannot
 * borrow the catalogue record's process lifetime. */
int main(void)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.studio");
    const UmiExperienceLayoutDefinition *layout;
    UmiExperienceLayoutDefinition copied_layout;
    UmiUiWorkspaceProfileSnapshot profile;

    UMI_TEST_REQUIRE(experience != NULL);
    layout = umi_application_experience_layout_find(
        experience, experience->default_layout_id);
    UMI_TEST_REQUIRE(layout != NULL);
    UMI_TEST_REQUIRE(umi_application_suite_layout_profile_project(
        experience, layout, 100, &profile) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(profile.sidebar_visible);
    UMI_TEST_REQUIRE(profile.auxiliary_sidebar_visible);
    UMI_TEST_REQUIRE(profile.bottom_panel_visible);
    UMI_TEST_REQUIRE(profile.pane_count > 0U);

    /* Copying the record preserves its text but not its canonical ownership. */
    copied_layout = *layout;
    UMI_TEST_REQUIRE(umi_application_suite_layout_profile_project(
        experience, &copied_layout, 100, &profile) == UMI_STATUS_NOT_FOUND);
    return 0;
}
