/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_browser_query_actions.c
 *
 * PURPOSE:
 *   Verify Layout Browser query parsing, filtering, sorting, selection and
 *   action preparation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchDesignerBrowser browser;
    UmiWorkbenchDesignerBrowserQuery query;
    UmiWorkbenchLayoutRecordSummary summary;
    UmiWorkbenchDesignerBrowserActionPlan action;
    const UmiWorkbenchDesignerBrowserItem *selected;
    size_t index;

    umi_workbench_designer_browser_init(&browser);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < 4U; ++index) {
        (void)memset(&summary, 0, sizeof(summary));
        summary.structure_size = sizeof(summary);
        (void)snprintf(
            summary.layout_id, sizeof(summary.layout_id), "layout.%zu", index);
        (void)snprintf(
            summary.name, sizeof(summary.name), "Layout %zu", index);
        TEST_REQUIRE_STATUS(test_copy_text(
            summary.category, sizeof(summary.category),
            index % 2U == 0U ? "development" : "trading"));
        TEST_REQUIRE_STATUS(test_copy_text(
            summary.owner_user_id, sizeof(summary.owner_user_id), "user.sammy"));
        summary.revision = index + 1U;
        summary.modified_at_ms = 1000U + index;
        TEST_REQUIRE_STATUS(umi_workbench_designer_browser_add_summary(
            &browser, &summary, "Reusable test layout", "test reusable",
            index == 0U));
    }
    TEST_REQUIRE_STATUS(umi_workbench_designer_browser_query_parse(
        "category:development Layout", &query));
    TEST_REQUIRE_STATUS(umi_workbench_designer_browser_set_query(
        &browser, &query));
    TEST_REQUIRE(browser.visible_count == 2U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_browser_set_sort(
        &browser, UMI_WORKBENCH_DESIGNER_BROWSER_SORT_REVISION, true));
    TEST_REQUIRE_STATUS(umi_workbench_designer_browser_select(
        &browser, "layout.2"));
    selected = umi_workbench_designer_browser_selected(&browser);
    TEST_REQUIRE(selected != NULL);
    TEST_REQUIRE(strcmp(selected->layout_id, "layout.2") == 0);
    umi_workbench_designer_browser_action_init(
        &action, UMI_WORKBENCH_DESIGNER_BROWSER_CLONE,
        "action.clone");
    TEST_REQUIRE_STATUS(umi_workbench_designer_browser_action_prepare(
        &browser, &action));
    TEST_REQUIRE(strcmp(action.layout_id, "layout.2") == 0);
    return 0;
}
