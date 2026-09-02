/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/runtime.h
 *
 * PURPOSE:
 *   Coordinate the compare, difference-map, directory, merge, partial-stage and
 *   review models as one reusable operational workbench runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKBENCH_RUNTIME_H
#define UMICOM_VCS_WORKBENCH_RUNTIME_H

#include "umicom/vcs/workbench/difference_map.h"
#include "umicom/vcs/workbench/directory_model.h"
#include "umicom/vcs/workbench/merge_model.h"
#include "umicom/vcs/workbench/partial_stage_model.h"
#include "umicom/vcs/workbench/review_session.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs workbench runtime data shared with callers of this public contract.
 */
typedef struct UmiVcsWorkbenchRuntime UmiVcsWorkbenchRuntime;

/**
 * Represent the vcs workbench runtime snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsWorkbenchRuntimeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsWorkbenchPhase phase;
    size_t compare_rows;
    size_t compare_hunks;
    size_t directory_entries;
    size_t visible_directory_entries;
    size_t merge_conflicts;
    size_t selected_stage_hunks;
    size_t review_threads;
    size_t unresolved_review_threads;
    int can_navigate_changes;
    int can_apply_partial_stage;
    int can_finish_merge;
    int can_approve_review;
    UmiStatus last_status;
    char last_message[UMI_VCS_WORKBENCH_TEXT_CAPACITY];
    uint64_t revision;
} UmiVcsWorkbenchRuntimeSnapshot;

/**
 * Initialise vcs workbench runtime from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_vcs_workbench_runtime_create(
    UmiVcsWorkbenchRuntime **out_runtime);
/**
 * Release or reset state held by vcs workbench runtime so the same storage can be reused
 * safely.
 */
void umi_vcs_workbench_runtime_destroy(
    UmiVcsWorkbenchRuntime *runtime);
/**
 * Provide the vcs workbench runtime open compare operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_runtime_open_compare(
    UmiVcsWorkbenchRuntime *runtime,
    const char *session_id,
    const UmiVcsAdvancedCompareSide *left,
    const UmiVcsAdvancedCompareSide *right,
    const char *left_text,
    const char *right_text,
    const UmiVcsAdvancedDiffOptions *options);
/**
 * Provide the vcs workbench runtime open merge operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_runtime_open_merge(
    UmiVcsWorkbenchRuntime *runtime,
    const char *session_id,
    const char *path,
    const char *base_text,
    const char *ours_text,
    const char *theirs_text);
/**
 * Provide the vcs workbench runtime begin directory compare operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workbench_runtime_begin_directory_compare(
    UmiVcsWorkbenchRuntime *runtime);
/**
 * Provide the vcs workbench runtime begin partial stage operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workbench_runtime_begin_partial_stage(
    UmiVcsWorkbenchRuntime *runtime,
    const char *repository_root,
    int reverse);
/**
 * Provide the vcs workbench runtime begin review operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workbench_runtime_begin_review(
    UmiVcsWorkbenchRuntime *runtime,
    const char *session_id);
/**
 * Provide the vcs workbench runtime snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workbench_runtime_snapshot(
    const UmiVcsWorkbenchRuntime *runtime,
    UmiVcsWorkbenchRuntimeSnapshot *out_snapshot);

/**
 * Provide the vcs workbench runtime compare operation used by this module and its client
 * applications.
 */
UmiVcsWorkbenchCompareModel *umi_vcs_workbench_runtime_compare(
    UmiVcsWorkbenchRuntime *runtime);
/**
 * Provide the vcs workbench runtime difference map operation used by this module and its
 * client applications.
 */
UmiVcsWorkbenchDifferenceMap *umi_vcs_workbench_runtime_difference_map(
    UmiVcsWorkbenchRuntime *runtime);
/**
 * Provide the vcs workbench runtime directory operation used by this module and its client
 * applications.
 */
UmiVcsWorkbenchDirectoryModel *umi_vcs_workbench_runtime_directory(
    UmiVcsWorkbenchRuntime *runtime);
/**
 * Provide the vcs workbench runtime merge operation used by this module and its client
 * applications.
 */
UmiVcsWorkbenchMergeModel *umi_vcs_workbench_runtime_merge(
    UmiVcsWorkbenchRuntime *runtime);
/**
 * Provide the vcs workbench runtime partial stage operation used by this module and its
 * client applications.
 */
UmiVcsWorkbenchPartialStageModel *umi_vcs_workbench_runtime_partial_stage(
    UmiVcsWorkbenchRuntime *runtime);
/**
 * Provide the vcs workbench runtime review operation used by this module and its client
 * applications.
 */
UmiVcsWorkbenchReviewSession *umi_vcs_workbench_runtime_review(
    UmiVcsWorkbenchRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_RUNTIME_H */
