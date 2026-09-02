/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/partial_stage_model.c
 *
 * PURPOSE:
 *   Implement workbench selection and summary state over the canonical
 *   reviewable partial-stage plan.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/partial_stage_model.h"

#include <string.h>

/* Provide the update ready operation used by this module and its client applications. */
static void update_ready(UmiVcsWorkbenchPartialStageModel *model)
{
    model->ready = umi_vcs_advanced_partial_stage_plan_ready(&model->plan);
}

/*
 * Initialise vcs workbench partial stage model from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_workbench_partial_stage_model_init(
    UmiVcsWorkbenchPartialStageModel *model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return;
    (void)memset(model, 0, sizeof(*model));
    model->struct_size = (uint32_t)sizeof(*model);
    model->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    model->revision = 1U;
    umi_vcs_advanced_partial_stage_plan_init(&model->plan);
}

/*
 * Provide the vcs workbench partial stage model begin operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_partial_stage_model_begin(
    UmiVcsWorkbenchPartialStageModel *model,
    const char *repository_root,
    int reverse)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_vcs_workbench_partial_stage_model_init(model);
    status = umi_vcs_advanced_partial_stage_plan_set_root(
        &model->plan, repository_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    model->plan.reverse = reverse != 0;
    model->plan.check_only = 1;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Add vcs workbench partial stage model only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_vcs_workbench_partial_stage_model_add(
    UmiVcsWorkbenchPartialStageModel *model,
    const UmiVcsAdvancedStagingHunk *hunk)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_partial_stage_plan_add(&model->plan, hunk);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    model->added_lines += hunk->added_lines;
    model->deleted_lines += hunk->deleted_lines;
    update_ready(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs workbench partial stage model select operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_partial_stage_model_select(
    UmiVcsWorkbenchPartialStageModel *model,
    size_t index,
    int selected)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_partial_stage_plan_select(
        &model->plan, index, selected);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    model->current_hunk = index;
    update_ready(model);
    model->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs workbench partial stage model select all operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workbench_partial_stage_model_select_all(
    UmiVcsWorkbenchPartialStageModel *model,
    int selected)
{
    size_t index;
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < model->plan.hunk_count;
         ++index) {
        status = umi_vcs_advanced_partial_stage_plan_select(
            &model->plan, index, selected);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        update_ready(model);
        model->revision += 1U;
    }
    return status;
}

/*
 * Find vcs workbench partial stage model hunk while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiVcsAdvancedStagingHunk *
umi_vcs_workbench_partial_stage_model_hunk_at(
    const UmiVcsWorkbenchPartialStageModel *model,
    size_t index)
{
    return model != NULL && index < model->plan.hunk_count
        ? &model->plan.hunks[index] : NULL;
}
