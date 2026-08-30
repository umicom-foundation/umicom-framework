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

static UmiEditorIntelApplicability action_applicability(
    const UmiEditorRankedCodeAction *action)
{
    if (!action->action.enabled) {
        return UMI_EDITOR_INTEL_APPLICABILITY_DISABLED;
    }
    return action->action.preferred
        ? UMI_EDITOR_INTEL_APPLICABILITY_PREFERRED
        : UMI_EDITOR_INTEL_APPLICABILITY_AVAILABLE;
}

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
    if (status != UMI_STATUS_OK) return status;
    if (action->action.preferred) flags |= UMI_EDITOR_INTEL_PROJECTION_PRIMARY;
    if (action->safe) flags |= UMI_EDITOR_INTEL_PROJECTION_SAFE;
    if (action->supports_preview) flags |= UMI_EDITOR_INTEL_PROJECTION_PREVIEW;
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

UmiStatus umi_editor_intel_code_action_projection_init(
    UmiEditorIntelCodeActionProjection *projection)
{
    if (projection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(projection, 0, sizeof(*projection));
    projection->struct_size = (uint32_t)sizeof(*projection);
    projection->api_version =
        UMI_EDITOR_INTEL_CODE_ACTION_PROJECTION_API_VERSION;
    if (umi_editor_intel_code_action_menu_group_init(&projection->menu) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    projection->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_intel_code_action_projection_refresh(
    UmiEditorIntelCodeActionProjection *projection,
    UmiEditorCodeActionOrchestration *orchestration)
{
    UmiEditorCodeActionQuery *query;
    size_t index;
    size_t count;
    UmiStatus status;

    if (projection == NULL || orchestration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    query = umi_editor_code_action_orchestration_query(orchestration);
    if (query == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_editor_code_action_query_snapshot(
        query, &projection->source_snapshot);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_editor_intel_code_action_menu_group_clear(&projection->menu);
    memset(projection->applicability, 0, sizeof(projection->applicability));
    memset(projection->provider_ids, 0, sizeof(projection->provider_ids));
    projection->has_selection = 0;
    projection->selected_index = 0U;
    count = projection->source_snapshot.result_count;
    if (count > UMI_EDITOR_INTEL_MAX_ITEMS) count = UMI_EDITOR_INTEL_MAX_ITEMS;
    for (index = 0U; index < count; ++index) {
        UmiEditorRankedCodeAction action;
        UmiEditorIntelEntry entry;

        status = umi_editor_code_action_query_at(query, index, &action);
        if (status != UMI_STATUS_OK) return status;
        status = action_entry(
            &entry, &action,
            projection->source_snapshot.request.document_uri,
            projection->source_snapshot.revision);
        if (status != UMI_STATUS_OK) return status;
        status = umi_editor_intel_code_action_menu_group_add(
            &projection->menu, &entry);
        if (status != UMI_STATUS_OK) return status;
        projection->applicability[index] = action_applicability(&action);
        status = umi_editor_intel_copy_text(
            projection->provider_ids[index],
            sizeof(projection->provider_ids[index]),
            action.provider_id);
        if (status != UMI_STATUS_OK) return status;
        if (!projection->has_selection && action.action.enabled) {
            projection->selected_index = index;
            projection->has_selection = 1;
        }
    }
    if (projection->has_selection) {
        projection->menu.items[projection->selected_index].flags |=
            UMI_EDITOR_INTEL_PROJECTION_SELECTED;
    }
    projection->source_revision = projection->source_snapshot.revision;
    ++projection->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_intel_code_action_projection_select(
    UmiEditorIntelCodeActionProjection *projection,
    size_t index)
{
    size_t item_index;

    if (projection == NULL || index >= projection->menu.count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (projection->applicability[index] ==
        UMI_EDITOR_INTEL_APPLICABILITY_DISABLED) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
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

const UmiEditorIntelEntry *umi_editor_intel_code_action_projection_selected(
    const UmiEditorIntelCodeActionProjection *projection)
{
    if (projection == NULL || !projection->has_selection ||
        projection->selected_index >= projection->menu.count) {
        return NULL;
    }
    return &projection->menu.items[projection->selected_index];
}

int umi_editor_intel_code_action_projection_valid(
    const UmiEditorIntelCodeActionProjection *projection)
{
    size_t index;

    if (projection == NULL ||
        projection->struct_size != (uint32_t)sizeof(*projection) ||
        projection->api_version !=
            UMI_EDITOR_INTEL_CODE_ACTION_PROJECTION_API_VERSION ||
        !umi_editor_intel_code_action_menu_group_valid(&projection->menu) ||
        (projection->has_selection &&
         projection->selected_index >= projection->menu.count)) {
        return 0;
    }
    for (index = 0U; index < projection->menu.count; ++index) {
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
