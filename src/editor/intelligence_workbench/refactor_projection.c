/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/refactor_projection.c
 *
 * PURPOSE:
 *   Implement canonical refactoring workbench projection and history state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/refactor_projection.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "umicom/editor/intelligence_workbench/projection.h"

/*
 * Provide the phase from snapshots operation used by this module and its client
 * applications.
 */
static UmiEditorIntelPhase phase_from_snapshots(
    const UmiEditorRefactoringPlanSnapshot *plan,
    const UmiEditorEditTransactionSnapshot *transaction)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (transaction->state) {
        case UMI_EDITOR_EDIT_TRANSACTION_APPLYING:
            return UMI_EDITOR_INTEL_PHASE_APPLYING;
        case UMI_EDITOR_EDIT_TRANSACTION_COMMITTED:
            return UMI_EDITOR_INTEL_PHASE_COMMITTED;
        case UMI_EDITOR_EDIT_TRANSACTION_ROLLED_BACK:
            return UMI_EDITOR_INTEL_PHASE_ROLLED_BACK;
        case UMI_EDITOR_EDIT_TRANSACTION_CANCELLED:
            return UMI_EDITOR_INTEL_PHASE_CANCELLED;
        case UMI_EDITOR_EDIT_TRANSACTION_PREFLIGHTED:
            return UMI_EDITOR_INTEL_PHASE_READY;
        case UMI_EDITOR_EDIT_TRANSACTION_CONFLICT:
            return UMI_EDITOR_INTEL_PHASE_READY;
        case UMI_EDITOR_EDIT_TRANSACTION_EMPTY:
            break;
        default:
            return UMI_EDITOR_INTEL_PHASE_IDLE;
    }
    /* Select the behaviour associated with the requested command or state value. */
    switch (plan->state) {
        case UMI_EDITOR_REFACTORING_PLAN_COLLECTING:
            return UMI_EDITOR_INTEL_PHASE_PREPARING;
        case UMI_EDITOR_REFACTORING_PLAN_READY:
        case UMI_EDITOR_REFACTORING_PLAN_CONFLICT:
            return UMI_EDITOR_INTEL_PHASE_READY;
        case UMI_EDITOR_REFACTORING_PLAN_APPLIED:
            return UMI_EDITOR_INTEL_PHASE_COMMITTED;
        case UMI_EDITOR_REFACTORING_PLAN_CANCELLED:
            return UMI_EDITOR_INTEL_PHASE_CANCELLED;
        case UMI_EDITOR_REFACTORING_PLAN_EMPTY:
        default:
            return UMI_EDITOR_INTEL_PHASE_IDLE;
    }
}

/* Provide the preview entry operation used by this module and its client applications. */
static UmiStatus preview_entry(
    UmiEditorIntelEntry *entry,
    const UmiEditorRefactoringPreviewItem *item,
    uint64_t revision)
{
    UmiEditorIntelProjectionFlags flags =
        UMI_EDITOR_INTEL_PROJECTION_VISIBLE |
        UMI_EDITOR_INTEL_PROJECTION_PREVIEW;
    const char *label;

    /* Apply this branch only when its contract condition is satisfied. */
    if (item->included) flags |= UMI_EDITOR_INTEL_PROJECTION_SELECTED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->conflict) flags |= UMI_EDITOR_INTEL_PROJECTION_CONFLICT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->edit.required) flags |= UMI_EDITOR_INTEL_PROJECTION_REQUIRED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (item->edit.state == UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED) {
        flags |= UMI_EDITOR_INTEL_PROJECTION_UNRESOLVED;
    }
    label = item->edit.replacement_text[0] != '\0'
        ? item->edit.replacement_text
        : "Delete source text";
    return umi_editor_intel_projection_entry_from_source(
        entry, item->edit.id, label, item->edit.expected_text,
        &item->edit.location, 0U, flags, revision);
}

/*
 * Initialise editor intel refactor projection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_refactor_projection_init(
    UmiEditorIntelRefactorProjection *projection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(projection, 0, sizeof(*projection));
    projection->struct_size = (uint32_t)sizeof(*projection);
    projection->api_version =
        UMI_EDITOR_INTEL_REFACTOR_PROJECTION_API_VERSION;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_intel_refactor_preview_model_init(&projection->preview) !=
            UMI_STATUS_OK ||
        umi_editor_intel_refactor_history_init(&projection->history) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    projection->phase = UMI_EDITOR_INTEL_PHASE_IDLE;
    projection->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor intel refactor projection refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_refactor_projection_refresh(
    UmiEditorIntelRefactorProjection *projection,
    UmiEditorCodeActionOrchestration *orchestration)
{
    UmiEditorRefactoringPlan *plan;
    UmiEditorRefactoringPreview *preview;
    UmiEditorEditTransaction *transaction;
    size_t index;
    size_t count;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || orchestration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    plan = umi_editor_code_action_orchestration_plan(orchestration);
    preview = umi_editor_code_action_orchestration_preview(orchestration);
    transaction = umi_editor_code_action_orchestration_transaction(orchestration);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || preview == NULL || transaction == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_editor_refactoring_plan_snapshot(
        plan, &projection->plan_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_refactoring_preview_snapshot(
        preview, &projection->preview_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_edit_transaction_snapshot(
        transaction, &projection->transaction_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_editor_intel_refactor_preview_model_clear(&projection->preview);
    projection->has_selection = 0;
    projection->selected_index = 0U;
    projection->has_conflict =
        projection->plan_snapshot.conflict_count > 0U ||
        projection->preview_snapshot.conflict_count > 0U ||
        projection->transaction_snapshot.state ==
            UMI_EDITOR_EDIT_TRANSACTION_CONFLICT;
    count = projection->preview_snapshot.item_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_EDITOR_INTEL_MAX_ITEMS) count = UMI_EDITOR_INTEL_MAX_ITEMS;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiEditorRefactoringPreviewItem item;
        UmiEditorIntelEntry entry;

        status = umi_editor_refactoring_preview_at(preview, index, &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = preview_entry(
            &entry, &item, projection->preview_snapshot.revision);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_editor_intel_refactor_preview_model_add(
            &projection->preview, &entry);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item.included && !projection->has_selection) {
            projection->selected_index = index;
            projection->has_selection = 1;
        }
    }
    projection->phase = phase_from_snapshots(
        &projection->plan_snapshot, &projection->transaction_snapshot);
    projection->source_revision = projection->plan_snapshot.revision ^
        projection->preview_snapshot.revision ^
        projection->transaction_snapshot.revision;
    ++projection->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor intel refactor projection set included operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_refactor_projection_set_included(
    UmiEditorIntelRefactorProjection *projection,
    UmiEditorCodeActionOrchestration *orchestration,
    size_t index,
    int included)
{
    UmiEditorRefactoringPreview *preview;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || orchestration == NULL ||
        index >= projection->preview.count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    preview = umi_editor_code_action_orchestration_preview(orchestration);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (preview == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_editor_refactoring_preview_set_included(
        preview, projection->preview.items[index].id, included);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_editor_intel_refactor_projection_refresh(
        projection, orchestration);
}

/*
 * Provide the editor intel refactor projection record history operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_intel_refactor_projection_record_history(
    UmiEditorIntelRefactorProjection *projection,
    const char *label)
{
    UmiEditorSourceLocation location;
    UmiEditorIntelEntry entry;
    char id[UMI_EDITOR_INTEL_ID_CAPACITY];
    const char *path;
    int written;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || label == NULL || label[0] == '\0' ||
        projection->plan_snapshot.descriptor.id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    path = projection->plan_snapshot.descriptor.document_uri[0] != '\0'
        ? projection->plan_snapshot.descriptor.document_uri
        : "workspace://refactoring";
    status = umi_editor_source_location_initialize(&location, path, 0U, 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    written = snprintf(
        id, sizeof(id), "refactor-%016" PRIx64,
        umi_editor_intel_projection_hash_text(
            projection->plan_snapshot.descriptor.id) ^ projection->revision);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_editor_intel_projection_entry_from_source(
        &entry, id, label, projection->plan_snapshot.descriptor.title,
        &location, (uint32_t)projection->phase,
        projection->has_conflict
            ? UMI_EDITOR_INTEL_PROJECTION_CONFLICT
            : UMI_EDITOR_INTEL_PROJECTION_VISIBLE,
        projection->revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_editor_intel_refactor_history_add(
        &projection->history, &entry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++projection->revision;
    return status;
}

/*
 * Find editor intel refactor projection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_refactor_projection_selected(
    const UmiEditorIntelRefactorProjection *projection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || !projection->has_selection ||
        projection->selected_index >= projection->preview.count) {
        return NULL;
    }
    return &projection->preview.items[projection->selected_index];
}

/*
 * Check that editor intel refactor projection satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_refactor_projection_valid(
    const UmiEditorIntelRefactorProjection *projection)
{
    return projection != NULL &&
        projection->struct_size == (uint32_t)sizeof(*projection) &&
        projection->api_version ==
            UMI_EDITOR_INTEL_REFACTOR_PROJECTION_API_VERSION &&
        projection->phase >= UMI_EDITOR_INTEL_PHASE_IDLE &&
        projection->phase <= UMI_EDITOR_INTEL_PHASE_CANCELLED &&
        umi_editor_intel_refactor_preview_model_valid(&projection->preview) &&
        umi_editor_intel_refactor_history_valid(&projection->history) &&
        (!projection->has_selection ||
         projection->selected_index < projection->preview.count);
}
