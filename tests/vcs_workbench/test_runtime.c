/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_workbench/test_runtime.c
 *
 * PURPOSE:
 *   Verify the unified runtime coordinates compare and review workbench state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/vcs/workbench/runtime.h"

int main(void)
{
    UmiVcsWorkbenchRuntime *runtime = NULL;
    UmiVcsWorkbenchRuntimeSnapshot snapshot;
    UmiVcsAdvancedCompareSide left;
    UmiVcsAdvancedCompareSide right;
    assert(umi_vcs_workbench_runtime_create(&runtime) == UMI_STATUS_OK);
    umi_vcs_advanced_compare_side_init(&left);
    umi_vcs_advanced_compare_side_init(&right);
    assert(umi_vcs_advanced_compare_side_set(
               &left, "a.c", "HEAD", "Left", 1) == UMI_STATUS_OK);
    assert(umi_vcs_advanced_compare_side_set(
               &right, "a.c", "", "Right", 0) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_runtime_open_compare(
               runtime, "runtime-compare", &left, &right,
               "a\nb\n", "a\nc\n", NULL) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_runtime_snapshot(runtime, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.phase == UMI_VCS_WORKBENCH_COMPARING);
    assert(snapshot.compare_hunks > 0U);
    assert(snapshot.can_navigate_changes);
    assert(umi_vcs_workbench_runtime_begin_review(
               runtime, "runtime-review") == UMI_STATUS_OK);
    assert(umi_vcs_workbench_runtime_snapshot(runtime, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.phase == UMI_VCS_WORKBENCH_REVIEWING);
    assert(snapshot.can_approve_review);
    umi_vcs_workbench_runtime_destroy(runtime);
    return 0;
}
