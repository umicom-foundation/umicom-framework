/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workbench/merge_model.c
 *
 * PURPOSE:
 *   Coordinate the established granular merge document and advanced
 *   three-way summary with explicit workbench selection and snapshots.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workbench/merge_model.h"

#include <stdlib.h>
#include <string.h>

struct UmiVcsWorkbenchMergeModel {
    UmiDeveloperMergeDocument *document;
    UmiVcsAdvancedThreeWayMergeResult canonical_result;
    char session_id[UMI_VCS_WORKBENCH_ID_CAPACITY];
    char path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    size_t initial_conflict_count;
    size_t current_block;
    int ready;
    uint64_t revision;
};

static UmiDeveloperMergeResolution map_choice(UmiVcsConflictChoice choice)
{
    switch (choice) {
        case UMI_VCS_CONFLICT_USE_BASE: return UMI_DEVELOPER_MERGE_USE_BASE;
        case UMI_VCS_CONFLICT_USE_OURS: return UMI_DEVELOPER_MERGE_USE_LEFT;
        case UMI_VCS_CONFLICT_USE_THEIRS: return UMI_DEVELOPER_MERGE_USE_RIGHT;
        case UMI_VCS_CONFLICT_USE_BOTH: return UMI_DEVELOPER_MERGE_USE_BOTH;
        case UMI_VCS_CONFLICT_UNRESOLVED:
        case UMI_VCS_CONFLICT_MANUAL:
        default: return UMI_DEVELOPER_MERGE_UNRESOLVED;
    }
}

UmiStatus umi_vcs_workbench_merge_model_create(
    UmiVcsWorkbenchMergeModel **out_model)
{
    UmiVcsWorkbenchMergeModel *model;

    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiVcsWorkbenchMergeModel *)calloc(1U, sizeof(*model));
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_vcs_workbench_merge_model_destroy(
    UmiVcsWorkbenchMergeModel *model)
{
    if (model == NULL) return;
    umi_developer_merge_document_destroy(model->document);
    free(model);
}

UmiStatus umi_vcs_workbench_merge_model_open(
    UmiVcsWorkbenchMergeModel *model,
    const char *session_id,
    const char *path,
    const char *base_text,
    const char *ours_text,
    const char *theirs_text)
{
    UmiDeveloperMergeDocument *document = NULL;
    UmiVcsAdvancedThreeWayMergeResult canonical_result;
    UmiStatus status;

    if (model == NULL || session_id == NULL || session_id[0] == '\0' ||
        path == NULL || path[0] == '\0' || base_text == NULL ||
        ours_text == NULL || theirs_text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_vcs_advanced_three_way_merge_text(
        base_text, ours_text, theirs_text, &canonical_result);
    if (status != UMI_STATUS_OK) return status;
    status = umi_developer_merge_document_create(
        base_text, ours_text, theirs_text, &document);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_workbench_copy_text(
        model->session_id, sizeof(model->session_id), session_id);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_workbench_copy_text(
            model->path, sizeof(model->path), path);
    }
    if (status != UMI_STATUS_OK) {
        umi_developer_merge_document_destroy(document);
        return status;
    }
    umi_developer_merge_document_destroy(model->document);
    model->document = document;
    model->canonical_result = canonical_result;
    model->initial_conflict_count =
        umi_developer_merge_document_conflict_count(document);
    model->current_block = 0U;
    model->ready = 1;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_merge_model_resolve(
    UmiVcsWorkbenchMergeModel *model,
    size_t block_index,
    UmiVcsConflictChoice choice)
{
    UmiDeveloperMergeResolution resolution;

    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    if (choice == UMI_VCS_CONFLICT_MANUAL) return UMI_STATUS_NOT_IMPLEMENTED;
    resolution = map_choice(choice);
    if (resolution == UMI_DEVELOPER_MERGE_UNRESOLVED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_developer_merge_document_resolve(
            model->document, block_index, resolution) != UMI_STATUS_OK) {
        return UMI_STATUS_NOT_FOUND;
    }
    model->current_block = block_index;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_merge_model_select(
    UmiVcsWorkbenchMergeModel *model,
    size_t block_index)
{
    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    if (block_index >=
        umi_developer_merge_document_block_count(model->document)) {
        return UMI_STATUS_NOT_FOUND;
    }
    model->current_block = block_index;
    model->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_merge_model_render(
    const UmiVcsWorkbenchMergeModel *model,
    char *out_text,
    size_t capacity,
    int include_conflict_markers)
{
    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_developer_merge_document_render(
        model->document, out_text, capacity, include_conflict_markers);
}

UmiStatus umi_vcs_workbench_merge_model_snapshot(
    const UmiVcsWorkbenchMergeModel *model,
    UmiVcsWorkbenchMergeSnapshot *out_snapshot)
{
    size_t conflicts;

    if (model == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_VCS_WORKBENCH_API_VERSION;
    if (!model->ready) return UMI_STATUS_OK;
    (void)umi_vcs_workbench_copy_text(
        out_snapshot->session_id, sizeof(out_snapshot->session_id),
        model->session_id);
    (void)umi_vcs_workbench_copy_text(
        out_snapshot->path, sizeof(out_snapshot->path), model->path);
    conflicts = umi_developer_merge_document_conflict_count(model->document);
    out_snapshot->block_count =
        umi_developer_merge_document_block_count(model->document);
    out_snapshot->conflict_count = conflicts;
    out_snapshot->resolved_count = model->initial_conflict_count >= conflicts
        ? model->initial_conflict_count - conflicts : 0U;
    out_snapshot->current_block = model->current_block;
    out_snapshot->complete = conflicts == 0U;
    out_snapshot->ready = 1;
    out_snapshot->result_fingerprint =
        model->canonical_result.fingerprint;
    out_snapshot->revision = model->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_workbench_merge_model_block_at(
    const UmiVcsWorkbenchMergeModel *model,
    size_t index,
    UmiDeveloperMergeBlock *out_block)
{
    if (model == NULL || !model->ready) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_developer_merge_document_block_at(
        model->document, index, out_block);
}
