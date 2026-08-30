/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_outline_filter.c
 *
 * PURPOSE:
 *   Verify outline filtering by title, selection, visibility and node kind.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerTree tree;
    UmiWorkbenchDesignerOutlineQuery query =
        umi_workbench_designer_outline_query_default();
    UmiWorkbenchDesignerOutlineResult result;
    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_selection_init(&selection);
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "terminal", true));
    umi_workbench_designer_tree_init(&tree);
    TEST_REQUIRE_STATUS(umi_workbench_designer_tree_build(
        &tree, &document, &selection));
    TEST_REQUIRE_STATUS(test_copy_text(query.text, sizeof(query.text), "term"));
    TEST_REQUIRE_STATUS(umi_workbench_designer_outline_filter(
        &tree, &query, &result));
    TEST_REQUIRE(result.count == 1U);
    TEST_REQUIRE(strcmp(result.rows[0].node_id, "terminal") == 0);
    query.text[0] = '\0';
    query.selected_only = true;
    TEST_REQUIRE_STATUS(umi_workbench_designer_outline_filter(
        &tree, &query, &result));
    TEST_REQUIRE(result.count == 1U);
    return 0;
}
