/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/symbol_relationship.c
 *
 * PURPOSE:
 *   Implement an owned symbol relationship graph and translate graph queries
 *   into the shared navigation-result model introduced by Batch 85.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/symbol_relationship.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorSymbolRelationshipGraph {
    UmiEditorSymbolRelationship *items;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus validate_relationship(
    const UmiEditorSymbolRelationship *relationship)
{
    if (relationship == NULL ||
        relationship->struct_size != (uint32_t)sizeof(*relationship) ||
        relationship->api_version !=
            UMI_EDITOR_SYMBOL_RELATIONSHIP_API_VERSION ||
        !terminated(relationship->id, sizeof(relationship->id)) ||
        relationship->id[0] == '\0' ||
        !terminated(relationship->source_symbol_id,
                    sizeof(relationship->source_symbol_id)) ||
        relationship->source_symbol_id[0] == '\0' ||
        !terminated(relationship->target_symbol_id,
                    sizeof(relationship->target_symbol_id)) ||
        relationship->target_symbol_id[0] == '\0' ||
        !terminated(relationship->provider_id,
                    sizeof(relationship->provider_id)) ||
        relationship->kind < UMI_EDITOR_SYMBOL_RELATIONSHIP_DEFINITION ||
        relationship->kind > UMI_EDITOR_SYMBOL_RELATIONSHIP_OVERRIDE ||
        umi_editor_source_location_validate(&relationship->source_location) !=
            UMI_STATUS_OK ||
        umi_editor_source_location_validate(&relationship->target_location) !=
            UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus reserve_relationships(
    UmiEditorSymbolRelationshipGraph *graph,
    size_t required)
{
    size_t capacity;
    UmiEditorSymbolRelationship *replacement;

    if (required <= graph->capacity) return UMI_STATUS_OK;
    capacity = graph->capacity > 0U ? graph->capacity : 256U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorSymbolRelationship *)realloc(
        graph->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    graph->items = replacement;
    graph->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_index(const UmiEditorSymbolRelationshipGraph *graph,
                         const char *relationship_id)
{
    size_t index;
    if (graph == NULL || relationship_id == NULL) return SIZE_MAX;
    for (index = 0U; index < graph->count; ++index) {
        if (strcmp(graph->items[index].id, relationship_id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_editor_symbol_relationship_graph_create(
    UmiEditorSymbolRelationshipGraph **out_graph)
{
    UmiEditorSymbolRelationshipGraph *graph;

    if (out_graph == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_graph = NULL;
    graph = (UmiEditorSymbolRelationshipGraph *)calloc(1U, sizeof(*graph));
    if (graph == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    graph->revision = 1U;
    *out_graph = graph;
    return UMI_STATUS_OK;
}

void umi_editor_symbol_relationship_graph_destroy(
    UmiEditorSymbolRelationshipGraph *graph)
{
    if (graph == NULL) return;
    free(graph->items);
    graph->items = NULL;
    free(graph);
}

UmiStatus umi_editor_symbol_relationship_graph_upsert(
    UmiEditorSymbolRelationshipGraph *graph,
    const UmiEditorSymbolRelationship *relationship)
{
    UmiEditorSymbolRelationship stored;
    size_t index;
    UmiStatus status;

    if (graph == NULL || validate_relationship(relationship) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(graph, relationship->id);
    if (index == SIZE_MAX) {
        status = reserve_relationships(graph, graph->count + 1U);
        if (status != UMI_STATUS_OK) return status;
        index = graph->count++;
    }
    stored = *relationship;
    graph->revision = next_revision(graph->revision);
    stored.revision = graph->revision;
    graph->items[index] = stored;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_symbol_relationship_graph_remove(
    UmiEditorSymbolRelationshipGraph *graph,
    const char *relationship_id)
{
    size_t index;

    if (graph == NULL || relationship_id == NULL ||
        relationship_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(graph, relationship_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < graph->count) {
        (void)memmove(graph->items + index,
                      graph->items + index + 1U,
                      (graph->count - index - 1U) * sizeof(*graph->items));
    }
    graph->count -= 1U;
    graph->revision = next_revision(graph->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_symbol_relationship_graph_remove_symbol(
    UmiEditorSymbolRelationshipGraph *graph,
    const char *symbol_id,
    size_t *out_removed_count)
{
    size_t read_index;
    size_t write_index = 0U;
    size_t removed = 0U;

    if (graph == NULL || symbol_id == NULL || symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (read_index = 0U; read_index < graph->count; ++read_index) {
        if (strcmp(graph->items[read_index].source_symbol_id, symbol_id) == 0 ||
            strcmp(graph->items[read_index].target_symbol_id, symbol_id) == 0) {
            removed += 1U;
            continue;
        }
        if (write_index != read_index) {
            graph->items[write_index] = graph->items[read_index];
        }
        write_index += 1U;
    }
    graph->count = write_index;
    if (removed > 0U) graph->revision = next_revision(graph->revision);
    if (out_removed_count != NULL) *out_removed_count = removed;
    return removed > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_symbol_relationship_graph_find(
    const UmiEditorSymbolRelationshipGraph *graph,
    const char *relationship_id,
    UmiEditorSymbolRelationship *out_relationship)
{
    size_t index;

    if (graph == NULL || relationship_id == NULL || out_relationship == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = find_index(graph, relationship_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_relationship = graph->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_symbol_relationship_graph_at(
    const UmiEditorSymbolRelationshipGraph *graph,
    size_t index,
    UmiEditorSymbolRelationship *out_relationship)
{
    if (graph == NULL || out_relationship == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= graph->count) return UMI_STATUS_NOT_FOUND;
    *out_relationship = graph->items[index];
    return UMI_STATUS_OK;
}

static UmiEditorSourceLocationKind location_kind(
    UmiEditorSymbolRelationshipKind kind)
{
    switch (kind) {
        case UMI_EDITOR_SYMBOL_RELATIONSHIP_DEFINITION:
            return UMI_EDITOR_SOURCE_LOCATION_DEFINITION;
        case UMI_EDITOR_SYMBOL_RELATIONSHIP_DECLARATION:
            return UMI_EDITOR_SOURCE_LOCATION_DECLARATION;
        case UMI_EDITOR_SYMBOL_RELATIONSHIP_IMPLEMENTATION:
            return UMI_EDITOR_SOURCE_LOCATION_IMPLEMENTATION;
        default:
            return UMI_EDITOR_SOURCE_LOCATION_REFERENCE;
    }
}

UmiStatus umi_editor_symbol_relationship_graph_collect(
    const UmiEditorSymbolRelationshipGraph *graph,
    const UmiEditorSymbolIndex *symbol_index,
    const char *symbol_id,
    UmiEditorSymbolRelationshipKind kind,
    int incoming,
    UmiEditorNavigationResultSet *out_results)
{
    size_t index;
    size_t added = 0U;

    if (graph == NULL || symbol_index == NULL || symbol_id == NULL ||
        symbol_id[0] == '\0' || out_results == NULL ||
        kind < UMI_EDITOR_SYMBOL_RELATIONSHIP_DEFINITION ||
        kind > UMI_EDITOR_SYMBOL_RELATIONSHIP_OVERRIDE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < graph->count; ++index) {
        const UmiEditorSymbolRelationship *edge = &graph->items[index];
        const char *match_id = incoming ? edge->target_symbol_id
                                        : edge->source_symbol_id;
        const char *result_id = incoming ? edge->source_symbol_id
                                         : edge->target_symbol_id;
        UmiEditorNavigationResult result = {0};
        UmiEditorIndexedSymbol symbol;
        UmiStatus status;

        if (edge->kind != kind || strcmp(match_id, symbol_id) != 0) continue;
        result.struct_size = (uint32_t)sizeof(result);
        result.api_version = UMI_EDITOR_NAVIGATION_RESULT_API_VERSION;
        {
            const char *provider = edge->provider_id[0] != '\0'
                ? edge->provider_id
                : "symbol-graph";
            (void)memcpy(result.provider_id,
                         provider,
                         strlen(provider) + 1U);
        }
        result.location = incoming ? edge->source_location
                                   : edge->target_location;
        result.location.kind = location_kind(edge->kind);
        result.rank = edge->rank;
        status = umi_editor_symbol_index_find(symbol_index, result_id, &symbol);
        if (status == UMI_STATUS_OK) {
            result.location = symbol.selection_location;
            result.location.kind = location_kind(edge->kind);
            (void)memcpy(result.location.symbol_id,
                         symbol.id,
                         strlen(symbol.id) + 1U);
            (void)memcpy(result.location.label,
                         symbol.name,
                         strlen(symbol.name) + 1U);
            (void)memcpy(result.detail,
                         symbol.detail,
                         strlen(symbol.detail) + 1U);
        }
        status = umi_editor_navigation_result_set_upsert(out_results, &result);
        if (status != UMI_STATUS_OK) return status;
        added += 1U;
    }
    if (added > 0U) return umi_editor_navigation_result_set_sort(out_results);
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_editor_symbol_relationship_graph_count(
    const UmiEditorSymbolRelationshipGraph *graph)
{
    return graph != NULL ? graph->count : 0U;
}

uint64_t umi_editor_symbol_relationship_graph_revision(
    const UmiEditorSymbolRelationshipGraph *graph)
{
    return graph != NULL ? graph->revision : 0U;
}
