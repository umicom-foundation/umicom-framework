/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/source_navigation.c
 *
 * PURPOSE:
 *   Coordinate Framework-owned symbol, relationship, bookmark and history
 *   services behind one reusable source-navigation boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/source_navigation.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorSourceNavigation {
    UmiEditorSymbolIndex *symbols;
    UmiEditorSymbolRelationshipGraph *relationships;
    UmiEditorNavigationHistory *history;
    UmiEditorSourceBookmarkRegistry *bookmarks;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

/* Provide the relationship kind operation used by this module and its client applications. */
static UmiEditorSymbolRelationshipKind relationship_kind(
    UmiEditorNavigationQueryKind query_kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (query_kind) {
        case UMI_EDITOR_NAVIGATION_QUERY_DEFINITION:
            return UMI_EDITOR_SYMBOL_RELATIONSHIP_DEFINITION;
        case UMI_EDITOR_NAVIGATION_QUERY_DECLARATION:
            return UMI_EDITOR_SYMBOL_RELATIONSHIP_DECLARATION;
        case UMI_EDITOR_NAVIGATION_QUERY_REFERENCE:
            return UMI_EDITOR_SYMBOL_RELATIONSHIP_REFERENCE;
        case UMI_EDITOR_NAVIGATION_QUERY_IMPLEMENTATION:
            return UMI_EDITOR_SYMBOL_RELATIONSHIP_IMPLEMENTATION;
        case UMI_EDITOR_NAVIGATION_QUERY_TYPE_DEFINITION:
            return UMI_EDITOR_SYMBOL_RELATIONSHIP_TYPE_DEFINITION;
        default:
            return 0;
    }
}

/*
 * Provide the source location kind operation used by this module and its client
 * applications.
 */
static UmiEditorSourceLocationKind source_location_kind(
    UmiEditorNavigationQueryKind query_kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (query_kind) {
        case UMI_EDITOR_NAVIGATION_QUERY_DEFINITION:
            return UMI_EDITOR_SOURCE_LOCATION_DEFINITION;
        case UMI_EDITOR_NAVIGATION_QUERY_DECLARATION:
            return UMI_EDITOR_SOURCE_LOCATION_DECLARATION;
        case UMI_EDITOR_NAVIGATION_QUERY_IMPLEMENTATION:
            return UMI_EDITOR_SOURCE_LOCATION_IMPLEMENTATION;
        default:
            return UMI_EDITOR_SOURCE_LOCATION_REFERENCE;
    }
}

/*
 * Provide the append indexed symbol operation used by this module and its client
 * applications.
 */
static UmiStatus append_indexed_symbol(
    UmiEditorNavigationResultSet *result_set,
    const UmiEditorIndexedSymbol *symbol,
    UmiEditorNavigationQueryKind query_kind)
{
    UmiEditorNavigationResult result = {0};

    result.struct_size = (uint32_t)sizeof(result);
    result.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
    {
        const char *provider = symbol->provider_id[0] != '\0'
            ? symbol->provider_id
            : "symbol-index";
        (void)memcpy(result.provider_id,
                     provider,
                     strlen(provider) + 1U);
    }
    (void)memcpy(result.detail,
                 symbol->detail,
                 strlen(symbol->detail) + 1U);
    result.location = symbol->selection_location;
    result.location.kind = source_location_kind(query_kind);
    (void)memcpy(result.location.symbol_id,
                 symbol->id,
                 strlen(symbol->id) + 1U);
    (void)memcpy(result.location.label,
                 symbol->name,
                 strlen(symbol->name) + 1U);
    result.rank = 1000;
    result.primary = 1;
    return umi_editor_navigation_result_set_upsert(result_set, &result);
}

/*
 * Initialise editor source navigation from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_source_navigation_create(
    UmiEditorSourceNavigation **out_navigation)
{
    UmiEditorSourceNavigation *navigation;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_navigation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_navigation = NULL;
    navigation = (UmiEditorSourceNavigation *)calloc(1U,
                                                      sizeof(*navigation));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_editor_symbol_index_create(0U, &navigation->symbols);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_symbol_relationship_graph_create(
            &navigation->relationships);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_navigation_history_create(0U,
                                                       &navigation->history);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_editor_source_bookmark_registry_create(
            &navigation->bookmarks);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_editor_source_navigation_destroy(navigation);
        return status;
    }
    navigation->revision = 1U;
    *out_navigation = navigation;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor source navigation so the same storage can be
 * reused safely.
 */
void umi_editor_source_navigation_destroy(
    UmiEditorSourceNavigation *navigation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL) return;
    umi_editor_source_bookmark_registry_destroy(navigation->bookmarks);
    umi_editor_navigation_history_destroy(navigation->history);
    umi_editor_symbol_relationship_graph_destroy(navigation->relationships);
    umi_editor_symbol_index_destroy(navigation->symbols);
    free(navigation);
}

/*
 * Provide the editor source navigation symbols operation used by this module and its
 * client applications.
 */
UmiEditorSymbolIndex *umi_editor_source_navigation_symbols(
    UmiEditorSourceNavigation *navigation)
{
    return navigation != NULL ? navigation->symbols : NULL;
}

/*
 * Provide the editor source navigation relationships operation used by this module and its
 * client applications.
 */
UmiEditorSymbolRelationshipGraph *umi_editor_source_navigation_relationships(
    UmiEditorSourceNavigation *navigation)
{
    return navigation != NULL ? navigation->relationships : NULL;
}

/*
 * Provide the editor source navigation history operation used by this module and its
 * client applications.
 */
UmiEditorNavigationHistory *umi_editor_source_navigation_history(
    UmiEditorSourceNavigation *navigation)
{
    return navigation != NULL ? navigation->history : NULL;
}

/*
 * Provide the editor source navigation bookmarks operation used by this module and its
 * client applications.
 */
UmiEditorSourceBookmarkRegistry *umi_editor_source_navigation_bookmarks(
    UmiEditorSourceNavigation *navigation)
{
    return navigation != NULL ? navigation->bookmarks : NULL;
}

/*
 * Provide the editor source navigation open operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_source_navigation_open(
    UmiEditorSourceNavigation *navigation,
    const UmiEditorSourceLocation *from,
    const UmiEditorSourceLocation *target,
    UmiEditorSourceLocation *out_target)
{
    UmiEditorSourceLocation current;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || out_target == NULL ||
        umi_editor_source_location_validate(target) != UMI_STATUS_OK ||
        (from != NULL &&
         umi_editor_source_location_validate(from) != UMI_STATUS_OK)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_navigation_history_current(navigation->history,
                                                    &current);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (from != NULL &&
        (status == UMI_STATUS_NOT_FOUND ||
         !umi_editor_source_location_same_position(&current, from))) {
        status = umi_editor_navigation_history_record(navigation->history,
                                                       from);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_editor_navigation_history_record(navigation->history,
                                                   target);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    *out_target = *target;
    navigation->revision = next_revision(navigation->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor source navigation resolve operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_navigation_resolve(
    UmiEditorSourceNavigation *navigation,
    const char *symbol_id,
    UmiEditorNavigationQueryKind query_kind,
    UmiEditorNavigationResultSet *out_results)
{
    UmiEditorSymbolRelationshipKind graph_kind;
    UmiEditorIndexedSymbol symbol;
    UmiStatus status;
    UmiStatus graph_status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || symbol_id == NULL || symbol_id[0] == '\0' ||
        out_results == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    graph_kind = relationship_kind(query_kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (graph_kind == 0) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_editor_navigation_result_set_reset(out_results, query_kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_editor_symbol_index_find(navigation->symbols,
                                           symbol_id,
                                           &symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        ((query_kind == UMI_EDITOR_NAVIGATION_QUERY_DEFINITION &&
          (symbol.flags & UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION) != 0U) ||
         (query_kind == UMI_EDITOR_NAVIGATION_QUERY_DECLARATION &&
          (symbol.flags & UMI_EDITOR_INDEXED_SYMBOL_FLAG_DECLARATION) != 0U))) {
        status = append_indexed_symbol(out_results, &symbol, query_kind);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    graph_status = umi_editor_symbol_relationship_graph_collect(
        navigation->relationships,
        navigation->symbols,
        symbol_id,
        graph_kind,
        0,
        out_results);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (graph_status != UMI_STATUS_OK &&
        graph_status != UMI_STATUS_NOT_FOUND) {
        return graph_status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_editor_navigation_result_set_count(out_results) == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }
    return umi_editor_navigation_result_set_sort(out_results);
}

/*
 * Provide the editor source navigation go back operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_navigation_go_back(
    UmiEditorSourceNavigation *navigation,
    UmiEditorSourceLocation *out_location)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_navigation_history_go_back(navigation->history,
                                                    out_location);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        navigation->revision = next_revision(navigation->revision);
    }
    return status;
}

/*
 * Provide the editor source navigation go forward operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_navigation_go_forward(
    UmiEditorSourceNavigation *navigation,
    UmiEditorSourceLocation *out_location)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_navigation_history_go_forward(navigation->history,
                                                       out_location);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        navigation->revision = next_revision(navigation->revision);
    }
    return status;
}

/*
 * Provide the editor source navigation snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_source_navigation_snapshot(
    const UmiEditorSourceNavigation *navigation,
    UmiEditorSourceNavigationSnapshot *out_snapshot)
{
    UmiEditorNavigationHistorySnapshot history_snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (navigation == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_editor_navigation_history_snapshot(navigation->history,
                                                     &history_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_SOURCE_NAVIGATION_API_VERSION;
    out_snapshot->symbol_count = umi_editor_symbol_index_count(
        navigation->symbols);
    out_snapshot->relationship_count =
        umi_editor_symbol_relationship_graph_count(navigation->relationships);
    out_snapshot->bookmark_count = umi_editor_source_bookmark_registry_count(
        navigation->bookmarks);
    out_snapshot->history_count = history_snapshot.count;
    out_snapshot->symbol_revision = umi_editor_symbol_index_revision(
        navigation->symbols);
    out_snapshot->relationship_revision =
        umi_editor_symbol_relationship_graph_revision(
            navigation->relationships);
    out_snapshot->bookmark_revision =
        umi_editor_source_bookmark_registry_revision(navigation->bookmarks);
    out_snapshot->history_revision = history_snapshot.revision;
    out_snapshot->revision = navigation->revision;
    out_snapshot->can_go_back = history_snapshot.can_go_back;
    out_snapshot->can_go_forward = history_snapshot.can_go_forward;
    return UMI_STATUS_OK;
}
