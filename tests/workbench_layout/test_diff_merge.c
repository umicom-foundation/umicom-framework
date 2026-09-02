/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_diff_merge.c
 *
 * PURPOSE:
 *   Verify semantic layout comparison and three-way merge conflict evidence.
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
    UmiWorkbenchLayoutDocument *base = test_allocate_layout("layout.base");
    UmiWorkbenchLayoutDocument *left =
        (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*left));
    UmiWorkbenchLayoutDocument *right =
        (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*right));
    UmiWorkbenchLayoutDocument *merged =
        (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*merged));
    UmiWorkbenchLayoutDiff *diff =
        (UmiWorkbenchLayoutDiff *)calloc(1U, sizeof(*diff));
    UmiWorkbenchLayoutMergePlan *merge =
        (UmiWorkbenchLayoutMergePlan *)calloc(1U, sizeof(*merge));
    UmiWorkbenchLayoutMergeOptions merge_options =
        umi_workbench_layout_merge_options_default();
    UmiWorkbenchLayoutNode *editor;
    UmiWorkbenchLayoutNode *output;

    TEST_REQUIRE(
        base != NULL && left != NULL && right != NULL && merged != NULL &&
        diff != NULL && merge != NULL,
        "Comparison state must be allocated");
    TEST_STATUS_OK(umi_workbench_layout_document_copy(left, base));
    TEST_STATUS_OK(umi_workbench_layout_document_copy(right, base));

    editor = umi_workbench_layout_document_find_node_mutable(left, "editor");
    output = umi_workbench_layout_document_find_node_mutable(right, "output");
    TEST_REQUIRE(editor != NULL && output != NULL, "Fixture panels must exist");
    TEST_STATUS_OK(umi_workbench_layout_node_set_context_group(
        editor, "blue.source-location"));
    TEST_STATUS_OK(umi_workbench_layout_node_set_context_group(
        output, "green.diagnostics"));
    umi_workbench_layout_document_refresh_hash(left);
    umi_workbench_layout_document_refresh_hash(right);

    TEST_STATUS_OK(umi_workbench_layout_compare(base, left, NULL, diff));
    TEST_REQUIRE(diff->entry_count > 0U, "Semantic diff must record the edit");
    TEST_REQUIRE(
        diff->changed_count == 1U,
        "One node must be reported as changed");

    TEST_STATUS_OK(umi_workbench_layout_merge_three_way(
        base, left, right, &merge_options, merge, merged));
    TEST_REQUIRE(merge->conflict_count == 0U, "Independent node edits must merge cleanly");
    TEST_REQUIRE(
        strcmp(
            umi_workbench_layout_document_find_node(
                merged, "editor")->context_group_id,
            "blue.source-location") == 0,
        "Left-side edit must survive merge");
    TEST_REQUIRE(
        strcmp(
            umi_workbench_layout_document_find_node(
                merged, "output")->context_group_id,
            "green.diagnostics") == 0,
        "Right-side edit must survive merge");

    merge_options.reject_unresolved_conflicts = false;
    TEST_STATUS_OK(umi_workbench_layout_node_set_title(editor, "Editor Left"));
    TEST_STATUS_OK(umi_workbench_layout_node_set_title(
        umi_workbench_layout_document_find_node_mutable(right, "editor"),
        "Editor Right"));
    umi_workbench_layout_document_refresh_hash(left);
    umi_workbench_layout_document_refresh_hash(right);
    TEST_STATUS_OK(umi_workbench_layout_merge_three_way(
        base, left, right, &merge_options, merge, merged));
    TEST_REQUIRE(merge->conflict_count > 0U, "Divergent edits must report a conflict");
    TEST_REQUIRE(merge->conflict_count > 0U, "Conflict evidence must be retained");

    free(merge);
    free(diff);
    free(merged);
    free(right);
    free(left);
    free(base);
    return 0;
}
