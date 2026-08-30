/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workspace_replace_plan.c
 *
 * PURPOSE:
 *   Build revision-aware, non-overlapping workspace edits from selected search
 *   replacement previews while revalidating source fingerprints and policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workspace_replace_plan.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorWorkspaceReplacePlan {
    UmiEditorWorkspaceEditSet *edit_set;
    UmiEditorWorkspaceReplacePlanSnapshot snapshot;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static void reset_snapshot(UmiEditorWorkspaceReplacePlan *plan)
{
    (void)memset(&plan->snapshot, 0, sizeof(plan->snapshot));
    plan->snapshot.struct_size = (uint32_t)sizeof(plan->snapshot);
    plan->snapshot.api_version = UMI_EDITOR_WORKSPACE_REPLACE_PLAN_API_VERSION;
    plan->snapshot.revision = plan->revision;
}

static int copy_text(char *destination,
                     size_t capacity,
                     const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return 0;
    length = strlen(source);
    if (length >= capacity) return 0;
    (void)memcpy(destination, source, length + 1U);
    return 1;
}

UmiStatus umi_editor_workspace_replace_plan_create(
    UmiEditorWorkspaceReplacePlan **out_plan)
{
    UmiEditorWorkspaceReplacePlan *plan;
    UmiStatus status;
    if (out_plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_plan = NULL;
    plan = (UmiEditorWorkspaceReplacePlan *)calloc(1U, sizeof(*plan));
    if (plan == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_workspace_edit_set_create(&plan->edit_set);
    if (status != UMI_STATUS_OK) {
        free(plan);
        return status;
    }
    plan->revision = 1U;
    reset_snapshot(plan);
    *out_plan = plan;
    return UMI_STATUS_OK;
}

void umi_editor_workspace_replace_plan_destroy(
    UmiEditorWorkspaceReplacePlan *plan)
{
    if (plan == NULL) return;
    umi_editor_workspace_edit_set_destroy(plan->edit_set);
    plan->edit_set = NULL;
    free(plan);
}

UmiStatus umi_editor_workspace_replace_plan_clear(
    UmiEditorWorkspaceReplacePlan *plan)
{
    UmiStatus status;
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_workspace_edit_set_clear(plan->edit_set);
    if (status != UMI_STATUS_OK) return status;
    plan->revision = next_revision(plan->revision);
    reset_snapshot(plan);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_replace_plan_build(
    UmiEditorWorkspaceReplacePlan *plan,
    const UmiEditorWorkspaceReplacementPreview *preview,
    const UmiEditorWorkspaceSearchIndex *index)
{
    UmiEditorWorkspaceReplacementPreviewSnapshot preview_snapshot;
    UmiEditorWorkspaceSearchIndexSnapshot index_snapshot;
    UmiEditorWorkspaceEditSnapshot edit_snapshot;
    size_t position;
    UmiStatus status;
    if (plan == NULL || preview == NULL || index == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_replacement_preview_snapshot(
        preview, &preview_snapshot);
    if (status != UMI_STATUS_OK || !preview_snapshot.ready) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_workspace_search_index_snapshot(index, &index_snapshot);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_workspace_edit_set_clear(plan->edit_set);
    if (status != UMI_STATUS_OK) return status;
    plan->revision = next_revision(plan->revision);
    reset_snapshot(plan);
    plan->snapshot.source_preview_revision = preview_snapshot.revision;
    plan->snapshot.source_index_revision = index_snapshot.revision;

    for (position = 0U; position < preview_snapshot.item_count; ++position) {
        UmiEditorWorkspaceReplacementPreviewItem item;
        UmiEditorWorkspaceSearchDocumentView document;
        UmiEditorWorkspaceTextEdit edit;
        status = umi_editor_workspace_replacement_preview_at(
            preview, position, &item);
        if (status != UMI_STATUS_OK) return status;
        if (!item.selected) {
            ++plan->snapshot.skipped_item_count;
            continue;
        }
        ++plan->snapshot.selected_item_count;
        if (!item.applicable || item.match.location.end_byte_offset <=
                                  item.match.location.byte_offset) {
            ++plan->snapshot.conflict_item_count;
            continue;
        }
        status = umi_editor_workspace_search_index_find_uri(
            index, item.match.location.uri, &document);
        if (status != UMI_STATUS_OK || document.read_only ||
            document.fingerprint != item.match.document_fingerprint ||
            (item.match.location.document_revision != 0U &&
             document.document_revision !=
                 item.match.location.document_revision)) {
            ++plan->snapshot.conflict_item_count;
            continue;
        }
        if (item.match.location.end_byte_offset > SIZE_MAX ||
            (size_t)item.match.location.end_byte_offset >
                document.content_length) {
            ++plan->snapshot.conflict_item_count;
            continue;
        }
        (void)memset(&edit, 0, sizeof(edit));
        edit.struct_size = (uint32_t)sizeof(edit);
        edit.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
        if (!copy_text(edit.id, sizeof(edit.id), item.match.id) ||
            !copy_text(edit.provider_id,
                       sizeof(edit.provider_id),
                       "umicom.workspace-search") ||
            !copy_text(edit.expected_text,
                       sizeof(edit.expected_text),
                       item.match.matched_text) ||
            !copy_text(edit.replacement_text,
                       sizeof(edit.replacement_text),
                       item.replacement_text)) {
            ++plan->snapshot.conflict_item_count;
            continue;
        }
        edit.location = item.match.location;
        edit.state = UMI_EDITOR_WORKSPACE_EDIT_READY;
        edit.required = 1;
        status = umi_editor_workspace_edit_set_upsert(plan->edit_set, &edit);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_editor_workspace_edit_set_finalize(plan->edit_set);
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_workspace_edit_set_snapshot(
        plan->edit_set, &edit_snapshot);
    if (status != UMI_STATUS_OK) return status;
    plan->snapshot.edit_count = edit_snapshot.edit_count;
    plan->snapshot.document_count = edit_snapshot.document_count;
    plan->snapshot.conflict_item_count += edit_snapshot.conflict_count;
    plan->snapshot.edit_set_revision = edit_snapshot.revision;
    plan->snapshot.finalized = edit_snapshot.finalized;
    plan->snapshot.applicable = edit_snapshot.applicable &&
        plan->snapshot.conflict_item_count == 0U &&
        plan->snapshot.edit_count > 0U;
    plan->snapshot.revision = plan->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_workspace_replace_plan_at(
    const UmiEditorWorkspaceReplacePlan *plan,
    size_t position,
    UmiEditorWorkspaceTextEdit *out_edit)
{
    if (plan == NULL || out_edit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_editor_workspace_edit_set_at(
        plan->edit_set, position, out_edit);
}

UmiStatus umi_editor_workspace_replace_plan_snapshot(
    const UmiEditorWorkspaceReplacePlan *plan,
    UmiEditorWorkspaceReplacePlanSnapshot *out_snapshot)
{
    if (plan == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = plan->snapshot;
    out_snapshot->revision = plan->revision;
    return UMI_STATUS_OK;
}

const UmiEditorWorkspaceEditSet *umi_editor_workspace_replace_plan_edit_set(
    const UmiEditorWorkspaceReplacePlan *plan)
{
    return plan != NULL ? plan->edit_set : NULL;
}

size_t umi_editor_workspace_replace_plan_count(
    const UmiEditorWorkspaceReplacePlan *plan)
{
    return plan != NULL
        ? umi_editor_workspace_edit_set_count(plan->edit_set) : 0U;
}

uint64_t umi_editor_workspace_replace_plan_revision(
    const UmiEditorWorkspaceReplacePlan *plan)
{
    return plan != NULL ? plan->revision : 0U;
}
