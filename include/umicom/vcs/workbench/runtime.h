/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workbench/runtime.h
 *
 * PURPOSE:
 *   Coordinate the compare, difference-map, directory, merge, partial-stage and
 *   review models as one reusable operational workbench runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiVcsWorkbenchRuntime UmiVcsWorkbenchRuntime;

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

UmiStatus umi_vcs_workbench_runtime_create(
    UmiVcsWorkbenchRuntime **out_runtime);
void umi_vcs_workbench_runtime_destroy(
    UmiVcsWorkbenchRuntime *runtime);
UmiStatus umi_vcs_workbench_runtime_open_compare(
    UmiVcsWorkbenchRuntime *runtime,
    const char *session_id,
    const UmiVcsAdvancedCompareSide *left,
    const UmiVcsAdvancedCompareSide *right,
    const char *left_text,
    const char *right_text,
    const UmiVcsAdvancedDiffOptions *options);
UmiStatus umi_vcs_workbench_runtime_open_merge(
    UmiVcsWorkbenchRuntime *runtime,
    const char *session_id,
    const char *path,
    const char *base_text,
    const char *ours_text,
    const char *theirs_text);
UmiStatus umi_vcs_workbench_runtime_begin_directory_compare(
    UmiVcsWorkbenchRuntime *runtime);
UmiStatus umi_vcs_workbench_runtime_begin_partial_stage(
    UmiVcsWorkbenchRuntime *runtime,
    const char *repository_root,
    int reverse);
UmiStatus umi_vcs_workbench_runtime_begin_review(
    UmiVcsWorkbenchRuntime *runtime,
    const char *session_id);
UmiStatus umi_vcs_workbench_runtime_snapshot(
    const UmiVcsWorkbenchRuntime *runtime,
    UmiVcsWorkbenchRuntimeSnapshot *out_snapshot);

UmiVcsWorkbenchCompareModel *umi_vcs_workbench_runtime_compare(
    UmiVcsWorkbenchRuntime *runtime);
UmiVcsWorkbenchDifferenceMap *umi_vcs_workbench_runtime_difference_map(
    UmiVcsWorkbenchRuntime *runtime);
UmiVcsWorkbenchDirectoryModel *umi_vcs_workbench_runtime_directory(
    UmiVcsWorkbenchRuntime *runtime);
UmiVcsWorkbenchMergeModel *umi_vcs_workbench_runtime_merge(
    UmiVcsWorkbenchRuntime *runtime);
UmiVcsWorkbenchPartialStageModel *umi_vcs_workbench_runtime_partial_stage(
    UmiVcsWorkbenchRuntime *runtime);
UmiVcsWorkbenchReviewSession *umi_vcs_workbench_runtime_review(
    UmiVcsWorkbenchRuntime *runtime);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_VCS_WORKBENCH_RUNTIME_H */
