/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_workbench/test_partial_stage_model.c
 *
 * PURPOSE:
 *   Verify preview-only selective staging over the canonical staging plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/vcs/workbench/partial_stage_model.h"

int main(void)
{
    UmiVcsWorkbenchPartialStageModel model;
    UmiVcsAdvancedStagingHunk hunk;
    assert(umi_vcs_workbench_partial_stage_model_begin(
               &model, ".", 0) == UMI_STATUS_OK);
    umi_vcs_advanced_staging_hunk_init(&hunk);
    assert(umi_vcs_advanced_staging_hunk_set(
               &hunk, "src/a.c", 1U, 2U, 1U, 3U,
               "@@ -1,2 +1,3 @@") == UMI_STATUS_OK);
    hunk.added_lines = 3U;
    hunk.deleted_lines = 2U;
    assert(umi_vcs_workbench_partial_stage_model_add(&model, &hunk) ==
           UMI_STATUS_OK);
    assert(model.ready);
    assert(model.added_lines == 3U);
    assert(umi_vcs_workbench_partial_stage_model_select(&model, 0U, 0) ==
           UMI_STATUS_OK);
    assert(!model.ready);
    assert(umi_vcs_workbench_partial_stage_model_select_all(&model, 1) ==
           UMI_STATUS_OK);
    assert(model.ready);
    return 0;
}
