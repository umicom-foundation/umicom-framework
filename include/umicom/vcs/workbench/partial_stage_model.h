/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/partial_stage_model.h
 *
 * PURPOSE:
 *   Add workbench selection, readiness and revision state around the canonical
 *   partial-stage plan without applying repository mutations directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_PARTIAL_STAGE_MODEL_H
#define UMICOM_VCS_WORKBENCH_PARTIAL_STAGE_MODEL_H

#include "umicom/vcs/advanced/partial_stage_plan.h"
#include "umicom/vcs/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_vcs_workbench_partial_stage_model_init(
    UmiVcsWorkbenchPartialStageModel *model);
UmiStatus umi_vcs_workbench_partial_stage_model_begin(
    UmiVcsWorkbenchPartialStageModel *model,
    const char *repository_root,
    int reverse);
UmiStatus umi_vcs_workbench_partial_stage_model_add(
    UmiVcsWorkbenchPartialStageModel *model,
    const UmiVcsAdvancedStagingHunk *hunk);
UmiStatus umi_vcs_workbench_partial_stage_model_select(
    UmiVcsWorkbenchPartialStageModel *model,
    size_t index,
    int selected);
UmiStatus umi_vcs_workbench_partial_stage_model_select_all(
    UmiVcsWorkbenchPartialStageModel *model,
    int selected);
const UmiVcsAdvancedStagingHunk *
umi_vcs_workbench_partial_stage_model_hunk_at(
    const UmiVcsWorkbenchPartialStageModel *model,
    size_t index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_PARTIAL_STAGE_MODEL_H */
