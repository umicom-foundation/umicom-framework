/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_workbench/test_merge_model.c
 *
 * PURPOSE:
 *   Verify reviewable three-way merge state and conflict resolution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/vcs/workbench/merge_model.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsWorkbenchMergeModel *model = NULL;
    UmiVcsWorkbenchMergeSnapshot snapshot;
    char output[256];
    assert(umi_vcs_workbench_merge_model_create(&model) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_merge_model_open(
               model, "merge-1", "src/a.c",
               "value = 1;\n", "value = 2;\n", "value = 3;\n") ==
           UMI_STATUS_OK);
    assert(umi_vcs_workbench_merge_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.ready);
    assert(snapshot.conflict_count == 1U);
    assert(umi_vcs_workbench_merge_model_resolve(
               model, 0U, UMI_VCS_CONFLICT_USE_OURS) == UMI_STATUS_OK);
    assert(umi_vcs_workbench_merge_model_snapshot(model, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.complete);
    assert(umi_vcs_workbench_merge_model_render(
               model, output, sizeof(output), 0) == UMI_STATUS_OK);
    umi_vcs_workbench_merge_model_destroy(model);
    return 0;
}
