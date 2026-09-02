/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_layout_statistics.c
 *
 * PURPOSE:
 *   Verify structural complexity and per-application ownership statistics for a
 *   cross-application layout.
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
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerLayoutStatistics statistics;
    const UmiWorkbenchDesignerApplicationStatistic *studio;
    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_layout_statistics_init(&statistics);
    TEST_REQUIRE_STATUS(umi_workbench_designer_layout_statistics_build(
        &statistics, &document));
    TEST_REQUIRE(statistics.total_nodes == document.node_count);
    TEST_REQUIRE(statistics.panels >= 3U);
    TEST_REQUIRE(statistics.maximum_depth >= 2U);
    TEST_REQUIRE(statistics.complexity_score > 0.0);
    studio = umi_workbench_designer_layout_statistics_application(
        &statistics, "org.umicom.studio");
    TEST_REQUIRE(studio != NULL && studio->panel_count >= 3U);
    return 0;
}
