/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_workbench/test_compare_model.c
 *
 * PURPOSE:
 *   Verify canonical diff data is composed into one navigable compare model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/vcs/workbench/compare_model.h"

int main(void)
{
    UmiVcsWorkbenchCompareModel model;
    UmiVcsAdvancedCompareSide left;
    UmiVcsAdvancedCompareSide right;
    umi_vcs_advanced_compare_side_init(&left);
    umi_vcs_advanced_compare_side_init(&right);
    assert(umi_vcs_advanced_compare_side_set(
               &left, "src/a.c", "HEAD", "Working", 1) == UMI_STATUS_OK);
    assert(umi_vcs_advanced_compare_side_set(
               &right, "src/a.c", "", "Index", 0) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_compare_model_open(
               &model, "compare-1", &left, &right,
               "one\ntwo\nthree\n", "one\nchanged\nthree\nfour\n", NULL) ==
           UMI_STATUS_OK);
    assert(model.ready);
    assert(model.row_count > 0U);
    assert(model.hunk_count > 0U);
    assert(umi_vcs_workbench_compare_model_row_at(&model, 0U) != NULL);
    assert(umi_vcs_workbench_compare_model_hunk_at(&model, 0U) != NULL);
    assert(umi_vcs_workbench_compare_model_set_view_mode(
               &model, UMI_VCS_WORKBENCH_INLINE) == UMI_STATUS_OK);
    assert(model.view_mode == UMI_VCS_WORKBENCH_INLINE);
    return 0;
}
