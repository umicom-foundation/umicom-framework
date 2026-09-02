/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/intelligence_workbench/search_projection.c
 *
 * PURPOSE:
 *   Implement the canonical workspace-search workbench projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/intelligence_workbench/search_projection.h"

#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "umicom/editor/intelligence_workbench/projection.h"

/* Provide the path label operation used by this module and its client applications. */
static const char *path_label(const char *path)
{
    const char *slash;
    const char *backslash;
    const char *label;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || path[0] == '\0') return "Workspace";
    slash = strrchr(path, '/');
    backslash = strrchr(path, '\\');
    label = path;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (slash != NULL) label = slash + 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (backslash != NULL && backslash + 1 > label) label = backslash + 1;
    return label[0] == '\0' ? path : label;
}

/* Provide the add group operation used by this module and its client applications. */
static UmiStatus add_group(
    UmiEditorIntelSearchProjection *projection,
    const char *path,
    uint64_t source_revision)
{
    UmiEditorIntelEntry entry;
    char id[UMI_EDITOR_INTEL_ID_CAPACITY];
    int written;

    written = snprintf(
        id, sizeof(id), "search-group-%016" PRIx64,
        umi_editor_intel_projection_hash_text(path));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(id)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_intel_search_result_group_find(
            &projection->groups, id) != NULL) {
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_intel_projection_entry_without_source(
            &entry, id, path_label(path), path, 0U,
            UMI_EDITOR_INTEL_PROJECTION_EXPANDED |
                UMI_EDITOR_INTEL_PROJECTION_VISIBLE,
            source_revision) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_editor_intel_search_result_group_add(
        &projection->groups, &entry);
}

/* Provide the entry from match operation used by this module and its client applications. */
static UmiStatus entry_from_match(
    UmiEditorIntelEntry *entry,
    const UmiEditorWorkspaceSearchMatch *match)
{
    UmiEditorIntelProjectionFlags flags = UMI_EDITOR_INTEL_PROJECTION_VISIBLE;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (match->selected) flags |= UMI_EDITOR_INTEL_PROJECTION_SELECTED;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (match->read_only) flags |= UMI_EDITOR_INTEL_PROJECTION_READ_ONLY;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (match->generated) flags |= UMI_EDITOR_INTEL_PROJECTION_GENERATED;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (match->truncated_text) flags |= UMI_EDITOR_INTEL_PROJECTION_TRUNCATED;
    return umi_editor_intel_projection_entry_from_source(
        entry,
        match->id,
        match->matched_text[0] != '\0' ? match->matched_text : "Search match",
        match->line_preview,
        &match->location,
        match->ordinal_in_document > (size_t)UINT32_MAX
            ? UINT32_MAX
            : (uint32_t)match->ordinal_in_document,
        flags,
        match->index_revision);
}

/*
 * Initialise editor intel search projection from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_intel_search_projection_init(
    UmiEditorIntelSearchProjection *projection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(projection, 0, sizeof(*projection));
    projection->struct_size = (uint32_t)sizeof(*projection);
    projection->api_version = UMI_EDITOR_INTEL_SEARCH_PROJECTION_API_VERSION;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_intel_workspace_search_view_model_init(
            &projection->results) != UMI_STATUS_OK ||
        umi_editor_intel_search_result_group_init(
            &projection->groups) != UMI_STATUS_OK) {
        return UMI_STATUS_INTERNAL_ERROR;
    }
    projection->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor intel search projection refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_search_projection_refresh(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration)
{
    UmiEditorWorkspaceSearchQuery *query;
    UmiEditorWorkspaceSearchQuerySnapshot query_snapshot;
    size_t index;
    size_t limit;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || orchestration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_workspace_search_orchestration_snapshot(
        orchestration, &projection->source_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    query = umi_editor_workspace_search_orchestration_query(orchestration);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return UMI_STATUS_INVALID_STATE;
    status = umi_editor_workspace_search_query_snapshot(query, &query_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)umi_editor_intel_workspace_search_view_model_clear(
        &projection->results);
    (void)umi_editor_intel_search_result_group_clear(&projection->groups);
    projection->has_selection = 0;
    projection->selected_index = 0U;
    limit = query_snapshot.result_count;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (limit > UMI_EDITOR_INTEL_MAX_ITEMS) {
        limit = UMI_EDITOR_INTEL_MAX_ITEMS;
        projection->truncated = 1;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        projection->truncated = query_snapshot.truncated;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < limit; ++index) {
        UmiEditorWorkspaceSearchMatch match;
        UmiEditorIntelEntry entry;

        status = umi_editor_workspace_search_query_at(query, index, &match);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = add_group(
            projection, match.location.uri, query_snapshot.revision);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = entry_from_match(&entry, &match);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_editor_intel_workspace_search_view_model_add(
            &projection->results, &entry);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (match.selected && !projection->has_selection) {
            projection->selected_index = index;
            projection->has_selection = 1;
        }
    }
    projection->source_revision = query_snapshot.revision;
    ++projection->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the editor intel search projection select operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_intel_search_projection_select(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    size_t index)
{
    UmiEditorWorkspaceSearchQuery *query;
    size_t item_index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || orchestration == NULL ||
        index >= projection->results.count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    query = umi_editor_workspace_search_orchestration_query(orchestration);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (item_index = 0U; item_index < projection->results.count; ++item_index) {
        status = umi_editor_workspace_search_query_select(
            query, projection->results.items[item_index].id,
            item_index == index ? 1 : 0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_editor_intel_search_projection_refresh(
        projection, orchestration);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        projection->selected_index = index;
        projection->has_selection = 1;
        projection->results.items[index].flags |=
            UMI_EDITOR_INTEL_PROJECTION_SELECTED;
    }
    return status;
}

/*
 * Provide the editor intel search projection select next operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_intel_search_projection_select_next(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    int wrap)
{
    size_t next;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || projection->results.count == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }
    next = projection->has_selection ? projection->selected_index + 1U : 0U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (next >= projection->results.count) {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!wrap) return UMI_STATUS_NOT_FOUND;
        next = 0U;
    }
    return umi_editor_intel_search_projection_select(
        projection, orchestration, next);
}

/*
 * Provide the editor intel search projection select previous operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_intel_search_projection_select_previous(
    UmiEditorIntelSearchProjection *projection,
    UmiEditorWorkspaceSearchOrchestration *orchestration,
    int wrap)
{
    size_t previous;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || projection->results.count == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!projection->has_selection) {
        previous = projection->results.count - 1U;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (projection->selected_index > 0U) {
        previous = projection->selected_index - 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (!wrap) return UMI_STATUS_NOT_FOUND;
        previous = projection->results.count - 1U;
    }
    return umi_editor_intel_search_projection_select(
        projection, orchestration, previous);
}

/*
 * Find editor intel search projection while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiEditorIntelEntry *umi_editor_intel_search_projection_selected(
    const UmiEditorIntelSearchProjection *projection)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (projection == NULL || !projection->has_selection ||
        projection->selected_index >= projection->results.count) {
        return NULL;
    }
    return &projection->results.items[projection->selected_index];
}

/*
 * Check that editor intel search projection satisfies its contract before another service
 * relies on it.
 */
int umi_editor_intel_search_projection_valid(
    const UmiEditorIntelSearchProjection *projection)
{
    return projection != NULL &&
        projection->struct_size == (uint32_t)sizeof(*projection) &&
        projection->api_version == UMI_EDITOR_INTEL_SEARCH_PROJECTION_API_VERSION &&
        umi_editor_intel_workspace_search_view_model_valid(
            &projection->results) &&
        umi_editor_intel_search_result_group_valid(&projection->groups) &&
        (!projection->has_selection ||
         projection->selected_index < projection->results.count);
}
