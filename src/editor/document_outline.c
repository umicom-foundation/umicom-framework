/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/document_outline.c
 *
 * PURPOSE:
 *   Implement a flattened hierarchical outline that preserves parent-before-
 *   child order and derives frontend-ready visibility from expansion state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/document_outline.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorDocumentOutline {
    UmiEditorDocumentOutlineEntry *entries;
    size_t count;
    size_t capacity;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

/* Provide the reserve entries operation used by this module and its client applications. */
static UmiStatus reserve_entries(UmiEditorDocumentOutline *outline,
                                 size_t required)
{
    size_t capacity;
    UmiEditorDocumentOutlineEntry *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= outline->capacity) return UMI_STATUS_OK;
    capacity = outline->capacity > 0U ? outline->capacity : 128U;
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
    replacement = (UmiEditorDocumentOutlineEntry *)realloc(
        outline->entries, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    outline->entries = replacement;
    outline->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find symbol operation used by this module and its client applications. */
static size_t find_symbol(const UmiEditorIndexedSymbol *symbols,
                          size_t count,
                          const char *symbol_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (symbol_id == NULL || symbol_id[0] == '\0') return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(symbols[index].id, symbol_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the compare symbols operation used by this module and its client applications. */
static int compare_symbols(const void *left_value, const void *right_value)
{
    const UmiEditorIndexedSymbol *left =
        (const UmiEditorIndexedSymbol *)left_value;
    const UmiEditorIndexedSymbol *right =
        (const UmiEditorIndexedSymbol *)right_value;
    int order = umi_editor_source_location_compare(&left->location,
                                                    &right->location);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    return strcmp(left->id, right->id);
}

/* Return the number of records represented by child without changing their state. */
static size_t child_count(const UmiEditorIndexedSymbol *symbols,
                          size_t count,
                          const char *parent_id)
{
    size_t index;
    size_t children = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(symbols[index].parent_id, parent_id) == 0) children += 1U;
    }
    return children;
}

/* Provide the next child operation used by this module and its client applications. */
static size_t next_child(const UmiEditorIndexedSymbol *symbols,
                         const unsigned char *emitted,
                         size_t count,
                         const char *parent_id)
{
    size_t index;
    size_t best = SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (emitted[index] ||
            strcmp(symbols[index].parent_id, parent_id) != 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (best == SIZE_MAX || umi_editor_source_location_compare(
                &symbols[index].location, &symbols[best].location) < 0) {
            best = index;
        }
    }
    return best;
}

/* Provide the append hierarchy operation used by this module and its client applications. */
static UmiStatus append_hierarchy(UmiEditorDocumentOutline *outline,
                                  const UmiEditorIndexedSymbol *symbols,
                                  unsigned char *emitted,
                                  size_t symbol_count,
                                  size_t symbol_index,
                                  size_t depth)
{
    UmiEditorDocumentOutlineEntry entry = {0};
    size_t child;
    UmiStatus status;

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (depth > symbol_count || emitted[symbol_index]) return UMI_STATUS_OK;
    status = reserve_entries(outline, outline->count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    emitted[symbol_index] = 1U;
    entry.struct_size = (uint32_t)sizeof(entry);
    entry.api_version = UMI_EDITOR_DOCUMENT_OUTLINE_API_VERSION;
    (void)memcpy(entry.symbol_id,
                 symbols[symbol_index].id,
                 strlen(symbols[symbol_index].id) + 1U);
    (void)memcpy(entry.parent_symbol_id,
                 symbols[symbol_index].parent_id,
                 strlen(symbols[symbol_index].parent_id) + 1U);
    (void)memcpy(entry.label,
                 symbols[symbol_index].name,
                 strlen(symbols[symbol_index].name) + 1U);
    (void)memcpy(entry.detail,
                 symbols[symbol_index].detail,
                 strlen(symbols[symbol_index].detail) + 1U);
    entry.kind = symbols[symbol_index].kind;
    entry.location = symbols[symbol_index].selection_location;
    entry.range = symbols[symbol_index].location;
    entry.depth = depth;
    entry.child_count = child_count(symbols,
                                    symbol_count,
                                    symbols[symbol_index].id);
    entry.expanded = 1;
    entry.visible = 1;
    outline->entries[outline->count++] = entry;

    /* Visit each bounded item once so every record receives the same rule. */
    for (;;) {
        child = next_child(symbols,
                           emitted,
                           symbol_count,
                           symbols[symbol_index].id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (child == SIZE_MAX) break;
        status = append_hierarchy(outline,
                                  symbols,
                                  emitted,
                                  symbol_count,
                                  child,
                                  depth + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Provide the entry index operation used by this module and its client applications. */
static size_t entry_index(const UmiEditorDocumentOutline *outline,
                          const char *symbol_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < outline->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(outline->entries[index].symbol_id, symbol_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Provide the refresh visibility operation used by this module and its client
 * applications.
 */
static void refresh_visibility(UmiEditorDocumentOutline *outline)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < outline->count; ++index) {
        size_t parent;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (outline->entries[index].parent_symbol_id[0] == '\0') {
            outline->entries[index].visible = 1;
            continue;
        }
        parent = entry_index(outline,
                             outline->entries[index].parent_symbol_id);
        outline->entries[index].visible = parent != SIZE_MAX &&
            outline->entries[parent].visible &&
            outline->entries[parent].expanded;
    }
}

/*
 * Initialise editor document outline from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_document_outline_create(
    UmiEditorDocumentOutline **out_outline)
{
    UmiEditorDocumentOutline *outline;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_outline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_outline = NULL;
    outline = (UmiEditorDocumentOutline *)calloc(1U, sizeof(*outline));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (outline == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    outline->revision = 1U;
    *out_outline = outline;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor document outline so the same storage can be reused
 * safely.
 */
void umi_editor_document_outline_destroy(UmiEditorDocumentOutline *outline)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (outline == NULL) return;
    free(outline->entries);
    outline->entries = NULL;
    free(outline);
}

/*
 * Provide the editor document outline build operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_document_outline_build(
    UmiEditorDocumentOutline *outline,
    const UmiEditorSymbolIndex *symbol_index,
    const char *document_id)
{
    UmiEditorIndexedSymbol *symbols;
    unsigned char *emitted;
    size_t source_count;
    size_t symbol_count = 0U;
    size_t index;
    UmiStatus status = UMI_STATUS_OK;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (outline == NULL || symbol_index == NULL || document_id == NULL ||
        document_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    source_count = umi_editor_symbol_index_count(symbol_index);
    symbols = (UmiEditorIndexedSymbol *)calloc(
        source_count > 0U ? source_count : 1U, sizeof(*symbols));
    emitted = (unsigned char *)calloc(source_count > 0U ? source_count : 1U,
                                      sizeof(*emitted));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (symbols == NULL || emitted == NULL) {
        free(symbols);
        free(emitted);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < source_count; ++index) {
        UmiEditorIndexedSymbol symbol;
        status = umi_editor_symbol_index_at(symbol_index, index, &symbol);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(symbol.document_id, document_id) == 0) {
            symbols[symbol_count++] = symbol;
        }
    }
    outline->count = 0U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (symbol_count > 1U) {
            qsort(symbols,
                  symbol_count,
                  sizeof(*symbols),
                  compare_symbols);
        }
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < symbol_count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (symbols[index].parent_id[0] == '\0' ||
                find_symbol(symbols,
                            symbol_count,
                            symbols[index].parent_id) == SIZE_MAX) {
                status = append_hierarchy(outline,
                                          symbols,
                                          emitted,
                                          symbol_count,
                                          index,
                                          0U);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) break;
            }
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < symbol_count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!emitted[index]) {
                status = append_hierarchy(outline,
                                          symbols,
                                          emitted,
                                          symbol_count,
                                          index,
                                          0U);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) break;
            }
        }
    }
    free(symbols);
    free(emitted);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        refresh_visibility(outline);
        outline->revision = next_revision(outline->revision);
    }
    return status;
}

/*
 * Provide the editor document outline select operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_document_outline_select(
    UmiEditorDocumentOutline *outline,
    const char *symbol_id)
{
    size_t selected;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (outline == NULL || symbol_id == NULL || symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    selected = entry_index(outline, symbol_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < outline->count; ++index) {
        outline->entries[index].selected = index == selected;
    }
    outline->revision = next_revision(outline->revision);
    return UMI_STATUS_OK;
}

/* Provide the contains operation used by this module and its client applications. */
static int contains(const UmiEditorSourceLocation *range,
                    const UmiEditorSourceLocation *point)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(range->uri, point->uri) != 0 ||
        point->line < range->line || point->line > range->end_line) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (point->line == range->line && point->column < range->column) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (point->line == range->end_line && point->column > range->end_column) {
        return 0;
    }
    return 1;
}

/*
 * Provide the editor document outline select location operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_document_outline_select_location(
    UmiEditorDocumentOutline *outline,
    const UmiEditorSourceLocation *location)
{
    size_t best = SIZE_MAX;
    size_t best_depth = 0U;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (outline == NULL ||
        umi_editor_source_location_validate(location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < outline->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (contains(&outline->entries[index].range, location) &&
            (best == SIZE_MAX || outline->entries[index].depth >= best_depth)) {
            best = index;
            best_depth = outline->entries[index].depth;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (best == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return umi_editor_document_outline_select(outline,
                                               outline->entries[best].symbol_id);
}

/*
 * Provide the editor document outline set expanded operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_document_outline_set_expanded(
    UmiEditorDocumentOutline *outline,
    const char *symbol_id,
    int expanded)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (outline == NULL || symbol_id == NULL || symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = entry_index(outline, symbol_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    outline->entries[index].expanded = expanded != 0;
    refresh_visibility(outline);
    outline->revision = next_revision(outline->revision);
    return UMI_STATUS_OK;
}

/*
 * Find editor document outline entry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_editor_document_outline_entry_at(
    const UmiEditorDocumentOutline *outline,
    size_t index,
    UmiEditorDocumentOutlineEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (outline == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= outline->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = outline->entries[index];
    return UMI_STATUS_OK;
}

/*
 * Find editor document outline visible entry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_document_outline_visible_entry_at(
    const UmiEditorDocumentOutline *outline,
    size_t visible_index,
    UmiEditorDocumentOutlineEntry *out_entry)
{
    size_t index;
    size_t current = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (outline == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < outline->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!outline->entries[index].visible) continue;
        /* Apply this operation only while the related capability or state is available. */
        if (current == visible_index) {
            *out_entry = outline->entries[index];
            return UMI_STATUS_OK;
        }
        current += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Return the number of records represented by editor document outline without changing
 * their state.
 */
size_t umi_editor_document_outline_count(
    const UmiEditorDocumentOutline *outline)
{
    return outline != NULL ? outline->count : 0U;
}

/*
 * Return the number of records represented by editor document outline visible without
 * changing their state.
 */
size_t umi_editor_document_outline_visible_count(
    const UmiEditorDocumentOutline *outline)
{
    size_t count = 0U;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (outline == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < outline->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (outline->entries[index].visible) count += 1U;
    }
    return count;
}

/*
 * Provide the editor document outline revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_document_outline_revision(
    const UmiEditorDocumentOutline *outline)
{
    return outline != NULL ? outline->revision : 0U;
}
