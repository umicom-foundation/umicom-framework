/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/refactoring_preview.c
 *
 * PURPOSE:
 *   Implement deterministic refactoring previews, optional-edit selection and
 *   canonical selected workspace-edit materialisation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/refactoring_preview.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorRefactoringPreview {
    UmiEditorRefactoringPreviewItem *items;
    size_t count;
    size_t capacity;
    char plan_id[UMI_EDITOR_REFACTORING_ID_CAPACITY];
    uint64_t plan_revision;
    uint64_t revision;
    int ready;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the reserve items operation used by this module and its client applications. */
static UmiStatus reserve_items(UmiEditorRefactoringPreview *preview,
                               size_t required)
{
    size_t capacity;
    UmiEditorRefactoringPreviewItem *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= preview->capacity) return UMI_STATUS_OK;
    capacity = preview->capacity > 0U ? preview->capacity : 16U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorRefactoringPreviewItem *)realloc(
        preview->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    preview->items = replacement;
    preview->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find item operation used by this module and its client applications. */
static size_t find_item(const UmiEditorRefactoringPreview *preview,
                        const char *edit_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || edit_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < preview->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(preview->items[index].edit.id, edit_id) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Initialise editor refactoring preview from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_refactoring_preview_create(
    UmiEditorRefactoringPreview **out_preview)
{
    UmiEditorRefactoringPreview *preview;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_preview = NULL;
    preview = (UmiEditorRefactoringPreview *)calloc(1U, sizeof(*preview));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    preview->revision = 1U;
    *out_preview = preview;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor refactoring preview so the same storage can be
 * reused safely.
 */
void umi_editor_refactoring_preview_destroy(
    UmiEditorRefactoringPreview *preview)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return;
    free(preview->items);
    preview->items = NULL;
    free(preview);
}

/*
 * Provide the editor refactoring preview build operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_preview_build(
    UmiEditorRefactoringPreview *preview,
    const UmiEditorRefactoringPlan *plan)
{
    UmiEditorRefactoringPlanSnapshot plan_snapshot;
    const UmiEditorWorkspaceEditSet *edits;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_refactoring_plan_snapshot(plan, &plan_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this operation only while the related capability or state is available. */
    if (plan_snapshot.state != UMI_EDITOR_REFACTORING_PLAN_READY &&
        plan_snapshot.state != UMI_EDITOR_REFACTORING_PLAN_CONFLICT) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = reserve_items(preview, plan_snapshot.edit_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    edits = umi_editor_refactoring_plan_edit_set_const(plan);
    preview->count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan_snapshot.edit_count; ++index) {
        UmiEditorRefactoringPreviewItem item;
        (void)memset(&item, 0, sizeof(item));
        status = umi_editor_workspace_edit_set_at(edits, index, &item.edit);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        item.struct_size = (uint32_t)sizeof(item);
        item.api_version = UMI_EDITOR_REFACTORING_PREVIEW_API_VERSION;
        item.conflict = item.edit.state == UMI_EDITOR_WORKSPACE_EDIT_CONFLICT;
        item.selectable = !item.edit.required && !item.conflict;
        item.included = !item.conflict;
        preview->items[preview->count++] = item;
    }
    (void)memcpy(preview->plan_id, plan_snapshot.descriptor.id,
                 sizeof(preview->plan_id));
    preview->plan_revision = plan_snapshot.revision;
    preview->ready = 1;
    preview->revision = next_revision(preview->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor refactoring preview set included operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_refactoring_preview_set_included(
    UmiEditorRefactoringPreview *preview,
    const char *edit_id,
    int included)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || edit_id == NULL || edit_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!preview->ready) return UMI_STATUS_INVALID_STATE;
    index = find_item(preview, edit_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (preview->items[index].edit.required && !included) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (preview->items[index].conflict && included) {
        return UMI_STATUS_INVALID_STATE;
    }
    preview->items[index].included = included != 0;
    preview->revision = next_revision(preview->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor refactoring preview select all operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_preview_select_all(
    UmiEditorRefactoringPreview *preview,
    int included)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!preview->ready) return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < preview->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (preview->items[index].selectable) {
            preview->items[index].included = included != 0;
        }
    }
    preview->revision = next_revision(preview->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor refactoring preview while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_refactoring_preview_at(
    const UmiEditorRefactoringPreview *preview,
    size_t index,
    UmiEditorRefactoringPreviewItem *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || out_item == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!preview->ready) return UMI_STATUS_INVALID_STATE;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= preview->count) return UMI_STATUS_NOT_FOUND;
    *out_item = preview->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor refactoring preview selected edits operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_refactoring_preview_selected_edits(
    const UmiEditorRefactoringPreview *preview,
    UmiEditorWorkspaceEditSet *out_edit_set)
{
    size_t index;
    size_t selected = 0U;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || out_edit_set == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!preview->ready) return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < preview->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (preview->items[index].conflict) return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_workspace_edit_set_clear(out_edit_set);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < preview->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!preview->items[index].included || preview->items[index].conflict) {
            continue;
        }
        status = umi_editor_workspace_edit_set_upsert(
            out_edit_set, &preview->items[index].edit);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        ++selected;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (selected == 0U) return UMI_STATUS_NOT_FOUND;
    return umi_editor_workspace_edit_set_finalize(out_edit_set);
}

/*
 * Provide the editor refactoring preview snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_refactoring_preview_snapshot(
    const UmiEditorRefactoringPreview *preview,
    UmiEditorRefactoringPreviewSnapshot *out_snapshot)
{
    size_t index;
    size_t comparison;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_REFACTORING_PREVIEW_API_VERSION;
    (void)memcpy(out_snapshot->plan_id, preview->plan_id,
                 sizeof(out_snapshot->plan_id));
    out_snapshot->item_count = preview->count;
    out_snapshot->plan_revision = preview->plan_revision;
    out_snapshot->revision = preview->revision;
    out_snapshot->ready = preview->ready;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < preview->count; ++index) {
        int first_document = 1;
        /* Visit each bounded item once so every record receives the same rule. */
        for (comparison = 0U; comparison < index; ++comparison) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (strcmp(preview->items[index].edit.location.uri,
                       preview->items[comparison].edit.location.uri) == 0) {
                first_document = 0;
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (first_document) ++out_snapshot->document_count;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (preview->items[index].included) {
            ++out_snapshot->included_count;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            ++out_snapshot->excluded_count;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (preview->items[index].edit.required) {
            ++out_snapshot->required_count;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (preview->items[index].conflict) ++out_snapshot->conflict_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor refactoring preview without changing
 * their state.
 */
size_t umi_editor_refactoring_preview_count(
    const UmiEditorRefactoringPreview *preview)
{
    return preview != NULL ? preview->count : 0U;
}

/*
 * Provide the editor refactoring preview revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_refactoring_preview_revision(
    const UmiEditorRefactoringPreview *preview)
{
    return preview != NULL ? preview->revision : 0U;
}
