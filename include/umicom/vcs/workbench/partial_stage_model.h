/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/partial_stage_model.h
 *
 * PURPOSE:
 *   Add workbench selection, readiness and revision state around the canonical
 *   partial-stage plan without applying repository mutations directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_PARTIAL_STAGE_MODEL_H
#define UMICOM_VCS_WORKBENCH_PARTIAL_STAGE_MODEL_H

#include "umicom/vcs/advanced/partial_stage_plan.h"
#include "umicom/vcs/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs workbench partial stage model data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsWorkbenchPartialStageModel {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsAdvancedPartialStagePlan plan;
    size_t current_hunk;
    size_t added_lines;
    size_t deleted_lines;
    int ready;
    uint64_t revision;
} UmiVcsWorkbenchPartialStageModel;

/**
 * Initialise vcs workbench partial stage model from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_workbench_partial_stage_model_init(
    UmiVcsWorkbenchPartialStageModel *model);
/**
 * Provide the vcs workbench partial stage model begin operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_partial_stage_model_begin(
    UmiVcsWorkbenchPartialStageModel *model,
    const char *repository_root,
    int reverse);
/**
 * Add vcs workbench partial stage model only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_vcs_workbench_partial_stage_model_add(
    UmiVcsWorkbenchPartialStageModel *model,
    const UmiVcsAdvancedStagingHunk *hunk);
/**
 * Provide the vcs workbench partial stage model select operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_partial_stage_model_select(
    UmiVcsWorkbenchPartialStageModel *model,
    size_t index,
    int selected);
/**
 * Provide the vcs workbench partial stage model select all operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workbench_partial_stage_model_select_all(
    UmiVcsWorkbenchPartialStageModel *model,
    int selected);
/**
 * Find vcs workbench partial stage model hunk while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiVcsAdvancedStagingHunk *
umi_vcs_workbench_partial_stage_model_hunk_at(
    const UmiVcsWorkbenchPartialStageModel *model,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_PARTIAL_STAGE_MODEL_H */
