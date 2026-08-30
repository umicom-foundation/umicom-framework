/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/merge_model.h
 *
 * PURPOSE:
 *   Coordinate the existing line-granular merge document and conservative
 *   three-way merge result as one reviewable conflict-resolution model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_MERGE_MODEL_H
#define UMICOM_VCS_WORKBENCH_MERGE_MODEL_H

#include "umicom/developer_productivity/merge.h"
#include "umicom/vcs/advanced/three_way_merge.h"
#include "umicom/vcs/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsWorkbenchMergeModel UmiVcsWorkbenchMergeModel;

typedef struct UmiVcsWorkbenchMergeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char session_id[UMI_VCS_WORKBENCH_ID_CAPACITY];
    char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    size_t block_count;
    size_t conflict_count;
    size_t resolved_count;
    size_t current_block;
    int complete;
    int ready;
    uint64_t result_fingerprint;
    uint64_t revision;
} UmiVcsWorkbenchMergeSnapshot;

UmiStatus umi_vcs_workbench_merge_model_create(
    UmiVcsWorkbenchMergeModel **out_model);
void umi_vcs_workbench_merge_model_destroy(
    UmiVcsWorkbenchMergeModel *model);
UmiStatus umi_vcs_workbench_merge_model_open(
    UmiVcsWorkbenchMergeModel *model,
    const char *session_id,
    const char *path,
    const char *base_text,
    const char *ours_text,
    const char *theirs_text);
UmiStatus umi_vcs_workbench_merge_model_resolve(
    UmiVcsWorkbenchMergeModel *model,
    size_t block_index,
    UmiVcsConflictChoice choice);
UmiStatus umi_vcs_workbench_merge_model_select(
    UmiVcsWorkbenchMergeModel *model,
    size_t block_index);
UmiStatus umi_vcs_workbench_merge_model_render(
    const UmiVcsWorkbenchMergeModel *model,
    char *out_text,
    size_t capacity,
    int include_conflict_markers);
UmiStatus umi_vcs_workbench_merge_model_snapshot(
    const UmiVcsWorkbenchMergeModel *model,
    UmiVcsWorkbenchMergeSnapshot *out_snapshot);
UmiStatus umi_vcs_workbench_merge_model_block_at(
    const UmiVcsWorkbenchMergeModel *model,
    size_t index,
    UmiDeveloperMergeBlock *out_block);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_MERGE_MODEL_H */
