/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_designer/test_diff_merge_review.c
 *
 * PURPOSE:
 *   Verify layout differences and merge conflicts project into stable review
 *   surfaces.
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
    static UmiWorkbenchLayoutDocument base;
    static UmiWorkbenchLayoutDocument local;
    static UmiWorkbenchLayoutDocument remote;
    static UmiWorkbenchLayoutDiff diff;
    static UmiWorkbenchLayoutMergePlan merge;
    static UmiWorkbenchLayoutDocument merged;
    UmiWorkbenchLayoutDiffOptions diff_options;
    UmiWorkbenchLayoutMergeOptions merge_options;
    static UmiWorkbenchDesignerDiffView diff_view;
    static UmiWorkbenchDesignerMergeView merge_view;
    static UmiWorkbenchDesignerReview review;
    UmiWorkbenchLayoutNode *node;

    TEST_REQUIRE_STATUS(test_make_document(&base));
    TEST_REQUIRE_STATUS(umi_workbench_layout_document_copy(&local, &base));
    TEST_REQUIRE_STATUS(umi_workbench_layout_document_copy(&remote, &base));
    node = umi_workbench_layout_document_find_node_mutable(&local, "editor");
    TEST_REQUIRE(node != NULL);
    TEST_REQUIRE_STATUS(umi_workbench_layout_node_set_title(
        node, "Local Editor"));
    umi_workbench_layout_document_increment_revision(&local);
    node = umi_workbench_layout_document_find_node_mutable(&remote, "editor");
    TEST_REQUIRE(node != NULL);
    TEST_REQUIRE_STATUS(umi_workbench_layout_node_set_title(
        node, "Remote Editor"));
    umi_workbench_layout_document_increment_revision(&remote);
    diff_options = umi_workbench_layout_diff_options_default();
    TEST_REQUIRE_STATUS(umi_workbench_layout_compare(
        &base, &local, &diff_options, &diff));
    umi_workbench_designer_diff_view_init(&diff_view);
    TEST_REQUIRE_STATUS(umi_workbench_designer_diff_view_build(
        &diff_view, &diff));
    TEST_REQUIRE(diff_view.count > 0U);
    merge_options = umi_workbench_layout_merge_options_default();
    merge_options.reject_unresolved_conflicts = false;
    TEST_REQUIRE_STATUS(umi_workbench_layout_merge_three_way(
        &base, &local, &remote, &merge_options, &merge, &merged));
    umi_workbench_designer_merge_view_init(&merge_view);
    TEST_REQUIRE_STATUS(umi_workbench_designer_merge_view_build(
        &merge_view, &merge));
    TEST_REQUIRE(merge_view.count > 0U);
    TEST_REQUIRE_STATUS(umi_workbench_designer_review_build(
        &review, &base, &local));
    TEST_REQUIRE(review.structural_change_count > 0U ||
                 review.metadata_change_count > 0U);
    TEST_REQUIRE(merge_view.count == merge.conflict_count);
    return 0;
}
