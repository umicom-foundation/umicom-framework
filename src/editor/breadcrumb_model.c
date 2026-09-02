/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/breadcrumb_model.c
 *
 * PURPOSE:
 *   Build workspace, path and enclosing-symbol breadcrumbs with stable source
 *   targets and sibling metadata suitable for keyboard or pointer navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/breadcrumb_model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorBreadcrumbModel {
    UmiEditorBreadcrumbSegment segments[UMI_EDITOR_BREADCRUMB_SEGMENT_CAPACITY];
    size_t count;
    size_t active_index;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the append segment operation used by this module and its client applications. */
static UmiStatus append_segment(UmiEditorBreadcrumbModel *model,
                                UmiEditorBreadcrumbSegmentKind kind,
                                const char *id,
                                const char *label,
                                UmiEditorIndexedSymbolKind symbol_kind,
                                const UmiEditorSourceLocation *location,
                                size_t sibling_count)
{
    UmiEditorBreadcrumbSegment segment = {0};
    UmiStatus status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count >= UMI_EDITOR_BREADCRUMB_SEGMENT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    segment.struct_size = (uint32_t)sizeof(segment);
    segment.api_version = UMI_EDITOR_BREADCRUMB_API_VERSION;
    segment.kind = kind;
    segment.symbol_kind = symbol_kind;
    segment.depth = model->count;
    segment.sibling_count = sibling_count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (location != NULL) segment.location = *location;
    status = copy_text(segment.id, sizeof(segment.id), id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(segment.label, sizeof(segment.label), label);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    model->segments[model->count++] = segment;
    return UMI_STATUS_OK;
}

/* Return the number of records represented by sibling without changing their state. */
static size_t sibling_count(const UmiEditorSymbolIndex *index,
                            const UmiEditorIndexedSymbol *symbol)
{
    size_t position;
    size_t count = 0U;
    size_t total = umi_editor_symbol_index_count(index);
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < total; ++position) {
        UmiEditorIndexedSymbol candidate;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_editor_symbol_index_at(index, position, &candidate) !=
            UMI_STATUS_OK) continue;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(candidate.document_id, symbol->document_id) == 0 &&
            strcmp(candidate.parent_id, symbol->parent_id) == 0) {
            count += 1U;
        }
    }
    return count;
}

/* Provide the chain contains operation used by this module and its client applications. */
static int chain_contains(
    char chain[UMI_EDITOR_BREADCRUMB_SEGMENT_CAPACITY]
              [UMI_EDITOR_SYMBOL_ID_CAPACITY],
    size_t chain_count,
    const char *symbol_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < chain_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(chain[index], symbol_id) == 0) return 1;
    }
    return 0;
}

/* Provide the append path operation used by this module and its client applications. */
static UmiStatus append_path(UmiEditorBreadcrumbModel *model,
                             const UmiEditorSourceLocation *location)
{
    char path[UMI_EDITOR_SOURCE_URI_CAPACITY];
    char *cursor;
    char *component;
    size_t component_index = 0U;
    UmiStatus status;

    status = copy_text(path, sizeof(path), location->uri);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    cursor = strstr(path, "://");
    cursor = cursor != NULL ? cursor + 3 : path;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor == '/' || *cursor == '\\') cursor += 1;
    component = cursor;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*component != '\0') {
        char *separator = component;
        char identifier[UMI_EDITOR_BREADCRUMB_ID_CAPACITY];
        int has_separator;
        int written;
        UmiEditorBreadcrumbSegmentKind kind;

        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*separator != '\0' && *separator != '/' && *separator != '\\') {
            separator += 1;
        }
        has_separator = *separator != '\0';
        /* Apply this branch only when its contract condition is satisfied. */
        if (has_separator) *separator = '\0';
        /* Apply this branch only when its contract condition is satisfied. */
        if (component[0] != '\0') {
            kind = has_separator ? UMI_EDITOR_BREADCRUMB_FOLDER
                                 : UMI_EDITOR_BREADCRUMB_FILE;
            written = snprintf(identifier,
                               sizeof(identifier),
                               "path:%zu:%s",
                               component_index,
                               component);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if ((size_t)written >= sizeof(identifier)) {
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            status = append_segment(model,
                                    kind,
                                    identifier,
                                    component,
                                    UMI_EDITOR_INDEXED_SYMBOL_UNKNOWN,
                                    location,
                                    0U);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            component_index += 1U;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (!has_separator) break;
        component = separator + 1;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*component == '/' || *component == '\\') component += 1;
    }
    return UMI_STATUS_OK;
}

/*
 * Initialise editor breadcrumb model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_breadcrumb_model_create(
    UmiEditorBreadcrumbModel **out_model)
{
    UmiEditorBreadcrumbModel *model;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_model = NULL;
    model = (UmiEditorBreadcrumbModel *)calloc(1U, sizeof(*model));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    model->active_index = SIZE_MAX;
    model->revision = 1U;
    *out_model = model;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor breadcrumb model so the same storage can be reused
 * safely.
 */
void umi_editor_breadcrumb_model_destroy(UmiEditorBreadcrumbModel *model)
{
    free(model);
}

/*
 * Provide the editor breadcrumb model build operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_breadcrumb_model_build(
    UmiEditorBreadcrumbModel *model,
    const UmiEditorSymbolIndex *symbol_index,
    const char *workspace_label,
    const UmiEditorSourceLocation *location)
{
    char chain[UMI_EDITOR_BREADCRUMB_SEGMENT_CAPACITY]
              [UMI_EDITOR_SYMBOL_ID_CAPACITY];
    size_t chain_count = 0U;
    UmiEditorIndexedSymbol symbol;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || symbol_index == NULL ||
        umi_editor_source_location_validate(location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    model->count = 0U;
    model->active_index = SIZE_MAX;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace_label != NULL && workspace_label[0] != '\0') {
        status = append_segment(model,
                                UMI_EDITOR_BREADCRUMB_WORKSPACE,
                                "workspace",
                                workspace_label,
                                UMI_EDITOR_INDEXED_SYMBOL_UNKNOWN,
                                location,
                                0U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    status = append_path(model, location);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_editor_symbol_index_find_enclosing(symbol_index,
                                                     location,
                                                     &symbol);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (status == UMI_STATUS_OK &&
           chain_count < UMI_EDITOR_BREADCRUMB_SEGMENT_CAPACITY) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (chain_contains(chain, chain_count, symbol.id)) break;
        status = copy_text(chain[chain_count],
                           sizeof(chain[chain_count]),
                           symbol.id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        chain_count += 1U;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (symbol.parent_id[0] == '\0') break;
        status = umi_editor_symbol_index_find(symbol_index,
                                               symbol.parent_id,
                                               &symbol);
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (chain_count > 0U) {
        chain_count -= 1U;
        status = umi_editor_symbol_index_find(symbol_index,
                                               chain[chain_count],
                                               &symbol);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = append_segment(model,
                                UMI_EDITOR_BREADCRUMB_SYMBOL,
                                symbol.id,
                                symbol.name,
                                symbol.kind,
                                &symbol.selection_location,
                                sibling_count(symbol_index, &symbol));
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (model->count > 0U) {
        model->active_index = model->count - 1U;
        model->segments[model->active_index].active = 1;
    }
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor breadcrumb model set active operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_breadcrumb_model_set_active(
    UmiEditorBreadcrumbModel *model,
    size_t index)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < model->count; ++position) {
        model->segments[position].active = position == index;
    }
    model->active_index = index;
    model->revision = next_revision(model->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor breadcrumb model while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_breadcrumb_model_at(
    const UmiEditorBreadcrumbModel *model,
    size_t index,
    UmiEditorBreadcrumbSegment *out_segment)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (model == NULL || out_segment == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= model->count) return UMI_STATUS_NOT_FOUND;
    *out_segment = model->segments[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor breadcrumb model without changing
 * their state.
 */
size_t umi_editor_breadcrumb_model_count(const UmiEditorBreadcrumbModel *model)
{
    return model != NULL ? model->count : 0U;
}

/*
 * Provide the editor breadcrumb model active index operation used by this module and its
 * client applications.
 */
size_t umi_editor_breadcrumb_model_active_index(
    const UmiEditorBreadcrumbModel *model)
{
    return model != NULL ? model->active_index : SIZE_MAX;
}

/*
 * Provide the editor breadcrumb model revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_breadcrumb_model_revision(
    const UmiEditorBreadcrumbModel *model)
{
    return model != NULL ? model->revision : 0U;
}
