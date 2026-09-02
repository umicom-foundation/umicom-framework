/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_clipboard.c
 *
 * PURPOSE:
 *   Verify semantic copy and paste preserve component ownership while assigning
 *   unique node identities.
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
    UmiWorkbenchDesignerSelection selection;
    UmiWorkbenchDesignerSelection pasted;
    UmiWorkbenchDesignerClipboard clipboard;
    size_t original_count;

    TEST_REQUIRE_STATUS(test_make_document(&document));
    original_count = document.node_count;
    umi_workbench_designer_selection_init(&selection);
    TEST_REQUIRE_STATUS(umi_workbench_designer_selection_add(
        &selection, "output", true));
    umi_workbench_designer_clipboard_init(&clipboard);
    TEST_REQUIRE_STATUS(umi_workbench_designer_clipboard_copy(
        &clipboard, &document, &selection, false));
    TEST_REQUIRE(!umi_workbench_designer_clipboard_empty(&clipboard));
    umi_workbench_designer_selection_init(&pasted);
    TEST_REQUIRE_STATUS(umi_workbench_designer_clipboard_paste(
        &clipboard, &document, "bottom", 2U, "copy", &pasted));
    TEST_REQUIRE(document.node_count == original_count + 1U);
    TEST_REQUIRE(pasted.count == 1U);
    TEST_REQUIRE(strcmp(
        umi_workbench_designer_selection_primary(&pasted), "output-copy") == 0);
    TEST_REQUIRE(umi_workbench_layout_document_find_node(
        &document, "output-copy") != NULL);
    return 0;
}
