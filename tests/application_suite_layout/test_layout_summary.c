/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_suite_layout/test_layout_summary.c
 *
 * PURPOSE:
 *   Verify one canonical layout exposes placement and customisation guidance
 *   without copying or reimplementing the application panel recipe.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience_catalogue.h"
#include "umicom/application/suite_layout/layout_summary.h"
#include "umicom/test_runtime/check.h"

/* Exercise a context-rich multi-monitor layout because it uses the broadest
 * set of public layout guidance fields. */
int main(void)
{
    const UmiApplicationExperienceDefinition *experience =
        umi_application_experience_catalogue_find("org.umicom.studio");
    const UmiExperienceLayoutDefinition *layout;
    UmiApplicationSuiteLayoutSummary summary;
    size_t placed_count;

    UMI_TEST_REQUIRE(experience != NULL);
    layout = umi_application_experience_layout_find(
        experience, "ai-assisted");
    UMI_TEST_REQUIRE(layout != NULL);
    UMI_TEST_REQUIRE(umi_application_suite_layout_summary_build(
        experience, layout, &summary) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(summary.definition == layout);
    UMI_TEST_REQUIRE(summary.panel_count == layout->panel_count);
    UMI_TEST_REQUIRE(summary.lockable);
    UMI_TEST_REQUIRE(summary.context_linked);
    UMI_TEST_REQUIRE(!summary.default_layout);
    placed_count = summary.left_count + summary.centre_count +
        summary.right_count + summary.top_count + summary.bottom_count +
        summary.floating_count;
    UMI_TEST_REQUIRE(placed_count == summary.panel_count);
    UMI_TEST_REQUIRE(summary.complete);
    return 0;
}
