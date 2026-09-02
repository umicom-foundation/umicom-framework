/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/code_action_projection.c
 *
 * PURPOSE:
 *   Implement canonical code-action result projection for workbench clients.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/code_action_projection.h"

#include <string.h>

#include "umicom/editor/intelligence_workbench/projection.h"

/*
 * Provide the action applicability operation used by this module and its client
 * applications.
 */
static UmiEditorIntelApplicability action_applicability(
    const UmiEditorRankedCodeAction *action)
{
    /* Apply this operation only while the related capability or state is available. */
    if (!action->action.enabled) {
        return UMI_EDITOR_INTEL_APPLICABILITY_DISABLED;
    }
    return action->action.preferred
        ? UMI_EDITOR_INTEL_APPLICABILITY_PREFERRED
        : UMI_EDITOR_INTEL_APPLICABILITY_AVAILABLE;
}

/* Provide the action entry operation used by this module and its client applications. */
static UmiStatus action_entry(
    UmiEditorIntelEntry *entry,
    const UmiEditorRankedCodeAction *action,
    const char *document_uri,
    uint64_t revision)
{
    UmiEditorSourceLocation source;
    UmiEditorIntelProjectionFlags flags =
        UMI_EDITOR_INTEL_PROJECTION_VISIBLE;
    const char *detail = action->disabled_reason[0] != '\0'
        ? action->disabled_reason
        : action->action.kind;
    UmiStatus status;

    status = umi_editor_source_location_initialize(
        &source,
        document_uri != NULL && document_uri[0] != '\0'
            ? document_uri
            : "workspace://code-actions",
        0U, 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (action->action.preferred) flags |= UMI_EDITOR_INTEL_PROJECTION_PRIMARY;
    /* Apply this branch only when its contract condition is satisfied. */
    if (action->safe) flags |= UMI_EDITOR_INTEL_PROJECTION_SAFE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (action->supports_preview) flags |= UMI_EDITOR_INTEL_PROJECTION_PREVIEW;
    /* Apply this branch only when its contract condition is satisfied. */
    if (action->requires_resolution) {
        flags |= UMI_EDITOR_INTEL_PROJECTION_UNRESOLVED;
    }
    return umi_editor_intel_projection_entry_from_source(
        entry,
        action->action.id,
        action->action.title,
        detail,
        &source,
        umi_editor_intel_projection_score_from_rank(action->relevance_score),
        flags,
        revision);
}

/*
 * Initialise editor intel code action projection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_code_action_projection_init(
    UmiEditorIntelCodeActionProjection *projection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(projection, 0, sizeof(*projection));
    projection->struct_size = (uint32_t)sizeof(*projection);
    projection->api_version =
        UMI_EDITOR_INTEL_CODE_ACTION_PROJECTION_API_VERSION;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_intel_code_action_menu_group_init(&projection->menu) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    projection->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor intel code action projection refresh operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_code_action_projection_refresh(
    UmiEditorIntelCodeActionProjection *projection,
    UmiEditorCodeActionOrchestration *orchestration)
{
    UmiEditorCodeActionQuery *query;
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
    query = umi_editor_code_action_orchestration_query(orchestration);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_editor_code_action_query_snapshot(
        query, &projection->source_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_editor_intel_code_action_menu_group_clear(&projection->menu);
    memset(projection->applicability, 0, sizeof(projection->applicability));
    memset(projection->provider_ids, 0, sizeof(projection->provider_ids));
    projection->has_selection = 0;
    projection->selected_index = 0U;
    count = projection->source_snapshot.result_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_EDITOR_INTEL_MAX_ITEMS) count = UMI_EDITOR_INTEL_MAX_ITEMS;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiEditorRankedCodeAction action;
        UmiEditorIntelEntry entry;

        status = umi_editor_code_action_query_at(query, index, &action);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = action_entry(
            &entry, &action,
            projection->source_snapshot.request.document_uri,
            projection->source_snapshot.revision);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_editor_intel_code_action_menu_group_add(
            &projection->menu, &entry);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        projection->applicability[index] = action_applicability(&action);
        status = umi_editor_intel_copy_text(
            projection->provider_ids[index],
            sizeof(projection->provider_ids[index]),
            action.provider_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this operation only while the related capability or state is available. */
        if (!projection->has_selection && action.action.enabled) {
            projection->selected_index = index;
            projection->has_selection = 1;
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (projection->has_selection) {
        projection->menu.items[projection->selected_index].flags |=
            UMI_EDITOR_INTEL_PROJECTION_SELECTED;
    }
    projection->source_revision = projection->source_snapshot.revision;
    ++projection->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor intel code action projection select operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_code_action_projection_select(
    UmiEditorIntelCodeActionProjection *projection,
    size_t index)
{
    size_t item_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || index >= projection->menu.count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (projection->applicability[index] ==
        UMI_EDITOR_INTEL_APPLICABILITY_DISABLED) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (item_index = 0U; item_index < projection->menu.count; ++item_index) {
        projection->menu.items[item_index].flags &=
            ~(uint32_t)UMI_EDITOR_INTEL_PROJECTION_SELECTED;
    }
    projection->menu.items[index].flags |= UMI_EDITOR_INTEL_PROJECTION_SELECTED;
    projection->selected_index = index;
    projection->has_selection = 1;
    ++projection->revision;
    return UMI_STATUS_OK;
}

/*
 * Find editor intel code action projection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_code_action_projection_selected(
    const UmiEditorIntelCodeActionProjection *projection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || !projection->has_selection ||
        projection->selected_index >= projection->menu.count) {
        return NULL;
    }
    return &projection->menu.items[projection->selected_index];
}

/*
 * Check that editor intel code action projection satisfies its contract before another
 * service relies on it.
 */
int umi_editor_intel_code_action_projection_valid(
    const UmiEditorIntelCodeActionProjection *projection)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL ||
        projection->struct_size != (uint32_t)sizeof(*projection) ||
        projection->api_version !=
            UMI_EDITOR_INTEL_CODE_ACTION_PROJECTION_API_VERSION ||
        !umi_editor_intel_code_action_menu_group_valid(&projection->menu) ||
        (projection->has_selection &&
         projection->selected_index >= projection->menu.count)) {
        return 0;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < projection->menu.count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (projection->provider_ids[index][0] == '\0' ||
            projection->applicability[index] <
                UMI_EDITOR_INTEL_APPLICABILITY_DISABLED ||
            projection->applicability[index] >
                UMI_EDITOR_INTEL_APPLICABILITY_PREFERRED) {
            return 0;
        }
    }
    return 1;
}
