/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_alignment_distribution.c
 *
 * PURPOSE:
 *   Verify selected node bounds can be aligned and distributed with revision
 *   evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"


int main(void)
{
    UmiWorkbenchLayoutDocument document;
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerAlignmentResult result;
    const UmiWorkbenchLayoutNode *output;
    const UmiWorkbenchLayoutNode *terminal;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_selection_init(&selection);
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "output", true));
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "terminal", false));
    TEST_REQUIRE_STATUS(umi_workbench_designer_align_selection(
        &document, &selection, UMI_WORKBENCH_DESIGNER_ALIGN_TOP, &result));
    TEST_REQUIRE(result.changed_count == 2U);
    output = umi_workbench_layout_document_find_node(&document, "output");
    terminal = umi_workbench_layout_document_find_node(&document, "terminal");
    TEST_REQUIRE(output != NULL && terminal != NULL);
    TEST_REQUIRE(output->bounds.y == terminal->bounds.y);

    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "project", false));
    TEST_REQUIRE_STATUS(umi_workbench_designer_distribute_selection(
        &document, &selection,
        UMI_WORKBENCH_DESIGNER_DISTRIBUTE_HORIZONTAL_CENTRES, &result));
    TEST_REQUIRE(result.resulting_revision >= result.previous_revision);
    return 0;
}
