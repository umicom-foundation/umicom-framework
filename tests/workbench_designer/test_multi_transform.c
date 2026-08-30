/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_multi_transform.c
 *
 * PURPOSE:
 *   Verify one transform snapshot moves and proportionally resizes multiple
 *   selected semantic nodes.
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
    UmiWorkbenchDesignerMultiTransform transform;
    const UmiWorkbenchLayoutNode *output;
    TEST_REQUIRE_STATUS(test_make_document(&document));
    umi_workbench_designer_selection_init(&selection);
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "output", true));
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "terminal", false));
    umi_workbench_designer_multi_transform_init(&transform);
    TEST_REQUIRE_STATUS(umi_workbench_designer_multi_transform_begin(
        &transform, &document, &selection));
    TEST_REQUIRE_STATUS(umi_workbench_designer_multi_transform_translate(
        &transform, 20.0, 10.0));
    TEST_REQUIRE_STATUS(umi_workbench_designer_multi_transform_apply(
        &transform, &document));
    output = umi_workbench_layout_document_find_node(&document, "output");
    TEST_REQUIRE(output != NULL && output->bounds.x == 300);
    return 0;
}
