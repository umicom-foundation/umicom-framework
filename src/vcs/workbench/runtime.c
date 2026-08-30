/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/runtime.c
 *
 * PURPOSE:
 *   Coordinate canonical compare/merge/staging/review models and expose one
 *   bounded operational snapshot to thin applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiVcsWorkbenchRuntime {
    UmiVcsWorkbenchCompareModel compare;
    UmiVcsWorkbenchDifferenceMap difference_map;
    UmiVcsWorkbenchDirectoryModel directory;
    UmiVcsWorkbenchMergeModel *merge;
    UmiVcsWorkbenchPartialStageModel partial_stage;
    UmiVcsWorkbenchReviewSession review;
    UmiVcsWorkbenchPhase phase;
    UmiStatus last_status;
    char last_message[UMI_VCS_WORKBENCH_TEXT_CAPACITY];
    uint64_t revision;
};

static UmiStatus finish(
    UmiVcsWorkbenchRuntime *runtime,
    UmiStatus status,
    UmiVcsWorkbenchPhase success_phase,
    const char *message)
{
    runtime->last_status = status;
    runtime->phase = status == UMI_STATUS_OK
        ? success_phase : UMI_VCS_WORKBENCH_FAILED;
    (void)snprintf(runtime->last_message, sizeof(runtime->last_message),
                   "%s", message != NULL ? message : umi_status_text(status));
    runtime->revision += 1U;
    return status;
}

UmiStatus umi_vcs_workbench_runtime_create(
    UmiVcsWorkbenchRuntime **out_runtime)
{
    UmiVcsWorkbenchRuntime *runtime;
    UmiStatus status;

    if (out_runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_runtime = NULL;
    runtime = (UmiVcsWorkbenchRuntime *)calloc(1U, sizeof(*runtime));
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_vcs_workbench_merge_model_create(&runtime->merge);
    if (status != UMI_STATUS_OK) {
        free(runtime);
        return status;
    }
    umi_vcs_workbench_compare_model_init(&runtime->compare);
    umi_vcs_workbench_difference_map_init(&runtime->difference_map);
    umi_vcs_workbench_directory_model_init(&runtime->directory);
    umi_vcs_workbench_partial_stage_model_init(&runtime->partial_stage);
    umi_vcs_workbench_review_session_init(&runtime->review);
    runtime->phase = UMI_VCS_WORKBENCH_IDLE;
    runtime->last_status = UMI_STATUS_OK;
    runtime->revision = 1U;
    (void)snprintf(runtime->last_message, sizeof(runtime->last_message),
                   "%s", "Diff and merge workbench ready.");
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

void umi_vcs_workbench_runtime_destroy(
    UmiVcsWorkbenchRuntime *runtime)
{
    if (runtime == NULL) return;
    umi_vcs_workbench_merge_model_destroy(runtime->merge);
    free(runtime);
}

UmiStatus umi_vcs_workbench_runtime_open_compare(
    UmiVcsWorkbenchRuntime *runtime,
    const char *session_id,
    const UmiVcsAdvancedCompareSide *left,
    const UmiVcsAdvancedCompareSide *right,
    const char *left_text,
    const char *right_text,
    const UmiVcsAdvancedDiffOptions *options)
{
    UmiStatus status;

    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_workbench_compare_model_open(
        &runtime->compare, session_id, left, right,
        left_text, right_text, options);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_workbench_difference_map_build(
            &runtime->difference_map, &runtime->compare);
    }
    return finish(runtime, status, UMI_VCS_WORKBENCH_COMPARING,
                  status == UMI_STATUS_OK
                      ? "Comparison opened." : "Comparison failed.");
}

UmiStatus umi_vcs_workbench_runtime_open_merge(
    UmiVcsWorkbenchRuntime *runtime,
    const char *session_id,
    const char *path,
    const char *base_text,
    const char *ours_text,
    const char *theirs_text)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_workbench_merge_model_open(
        runtime->merge, session_id, path, base_text, ours_text, theirs_text);
    return finish(runtime, status, UMI_VCS_WORKBENCH_MERGING,
                  status == UMI_STATUS_OK
                      ? "Merge session opened." : "Merge session failed.");
}

UmiStatus umi_vcs_workbench_runtime_begin_directory_compare(
    UmiVcsWorkbenchRuntime *runtime)
{
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_vcs_workbench_directory_model_init(&runtime->directory);
    return finish(runtime, UMI_STATUS_OK,
                  UMI_VCS_WORKBENCH_DIRECTORY_COMPARING,
                  "Directory comparison started.");
}

UmiStatus umi_vcs_workbench_runtime_begin_partial_stage(
    UmiVcsWorkbenchRuntime *runtime,
    const char *repository_root,
    int reverse)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_workbench_partial_stage_model_begin(
        &runtime->partial_stage, repository_root, reverse);
    return finish(runtime, status, UMI_VCS_WORKBENCH_PARTIAL_STAGING,
                  status == UMI_STATUS_OK
                      ? "Partial-stage preview started."
                      : "Partial-stage preview failed.");
}

UmiStatus umi_vcs_workbench_runtime_begin_review(
    UmiVcsWorkbenchRuntime *runtime,
    const char *session_id)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_workbench_review_session_begin(
        &runtime->review, session_id);
    return finish(runtime, status, UMI_VCS_WORKBENCH_REVIEWING,
                  status == UMI_STATUS_OK
                      ? "Review session started." : "Review session failed.");
}

UmiStatus umi_vcs_workbench_runtime_snapshot(
    const UmiVcsWorkbenchRuntime *runtime,
    UmiVcsWorkbenchRuntimeSnapshot *out_snapshot)
{
    UmiVcsWorkbenchMergeSnapshot merge_snapshot;

    if (runtime == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    out_snapshot->phase = runtime->phase;
    out_snapshot->compare_rows = runtime->compare.row_count;
    out_snapshot->compare_hunks = runtime->compare.hunk_count;
    out_snapshot->directory_entries = runtime->directory.count;
    out_snapshot->visible_directory_entries = runtime->directory.visible_count;
    if (umi_vcs_workbench_merge_model_snapshot(
            runtime->merge, &merge_snapshot) == UMI_STATUS_OK) {
        out_snapshot->merge_conflicts = merge_snapshot.conflict_count;
        out_snapshot->can_finish_merge =
            merge_snapshot.ready && merge_snapshot.complete;
    }
    out_snapshot->selected_stage_hunks =
        runtime->partial_stage.plan.selected_count;
    out_snapshot->review_threads = runtime->review.thread_count;
    out_snapshot->unresolved_review_threads =
        runtime->review.unresolved_count;
    out_snapshot->can_navigate_changes = runtime->compare.hunk_count > 0U;
    out_snapshot->can_apply_partial_stage = runtime->partial_stage.ready;
    out_snapshot->can_approve_review =
        runtime->review.ready && runtime->review.unresolved_count == 0U;
    out_snapshot->last_status = runtime->last_status;
    (void)snprintf(out_snapshot->last_message,
                   sizeof(out_snapshot->last_message), "%s",
                   runtime->last_message);
    out_snapshot->revision = runtime->revision;
    return UMI_STATUS_OK;
}

UmiVcsWorkbenchCompareModel *umi_vcs_workbench_runtime_compare(
    UmiVcsWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->compare : NULL;
}

UmiVcsWorkbenchDifferenceMap *umi_vcs_workbench_runtime_difference_map(
    UmiVcsWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->difference_map : NULL;
}

UmiVcsWorkbenchDirectoryModel *umi_vcs_workbench_runtime_directory(
    UmiVcsWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->directory : NULL;
}

UmiVcsWorkbenchMergeModel *umi_vcs_workbench_runtime_merge(
    UmiVcsWorkbenchRuntime *runtime)
{
    return runtime != NULL ? runtime->merge : NULL;
}

UmiVcsWorkbenchPartialStageModel *umi_vcs_workbench_runtime_partial_stage(
    UmiVcsWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->partial_stage : NULL;
}

UmiVcsWorkbenchReviewSession *umi_vcs_workbench_runtime_review(
    UmiVcsWorkbenchRuntime *runtime)
{
    return runtime != NULL ? &runtime->review : NULL;
}
