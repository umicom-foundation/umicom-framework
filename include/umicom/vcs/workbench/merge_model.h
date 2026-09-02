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

/**
 * Represent the vcs workbench merge model data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsWorkbenchMergeModel UmiVcsWorkbenchMergeModel;

/**
 * Represent the vcs workbench merge snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise vcs workbench merge model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_vcs_workbench_merge_model_create(
    UmiVcsWorkbenchMergeModel **out_model);
/**
 * Release or reset state held by vcs workbench merge model so the same storage can be
 * reused safely.
 */
void umi_vcs_workbench_merge_model_destroy(
    UmiVcsWorkbenchMergeModel *model);
/**
 * Provide the vcs workbench merge model open operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workbench_merge_model_open(
    UmiVcsWorkbenchMergeModel *model,
    const char *session_id,
    const char *path,
    const char *base_text,
    const char *ours_text,
    const char *theirs_text);
/**
 * Provide the vcs workbench merge model resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_merge_model_resolve(
    UmiVcsWorkbenchMergeModel *model,
    size_t block_index,
    UmiVcsConflictChoice choice);
/**
 * Provide the vcs workbench merge model select operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_merge_model_select(
    UmiVcsWorkbenchMergeModel *model,
    size_t block_index);
/**
 * Provide the vcs workbench merge model render operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_merge_model_render(
    const UmiVcsWorkbenchMergeModel *model,
    char *out_text,
    size_t capacity,
    int include_conflict_markers);
/**
 * Provide the vcs workbench merge model snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_merge_model_snapshot(
    const UmiVcsWorkbenchMergeModel *model,
    UmiVcsWorkbenchMergeSnapshot *out_snapshot);
/**
 * Find vcs workbench merge model block while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workbench_merge_model_block_at(
    const UmiVcsWorkbenchMergeModel *model,
    size_t index,
    UmiDeveloperMergeBlock *out_block);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_MERGE_MODEL_H */
