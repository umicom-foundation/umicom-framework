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

static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

static UmiStatus reserve_entries(UmiEditorDocumentOutline *outline,
                                 size_t required)
{
    size_t capacity;
    UmiEditorDocumentOutlineEntry *replacement;

    if (required <= outline->capacity) return UMI_STATUS_OK;
    capacity = outline->capacity > 0U ? outline->capacity : 128U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorDocumentOutlineEntry *)realloc(
        outline->entries, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    outline->entries = replacement;
    outline->capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_symbol(const UmiEditorIndexedSymbol *symbols,
                          size_t count,
                          const char *symbol_id)
{
    size_t index;
    if (symbol_id == NULL || symbol_id[0] == '\0') return SIZE_MAX;
    for (index = 0U; index < count; ++index) {
        if (strcmp(symbols[index].id, symbol_id) == 0) return index;
    }
    return SIZE_MAX;
}

static int compare_symbols(const void *left_value, const void *right_value)
{
    const UmiEditorIndexedSymbol *left =
        (const UmiEditorIndexedSymbol *)left_value;
    const UmiEditorIndexedSymbol *right =
        (const UmiEditorIndexedSymbol *)right_value;
    int order = umi_editor_source_location_compare(&left->location,
                                                    &right->location);
    if (order != 0) return order;
    return strcmp(left->id, right->id);
}

static size_t child_count(const UmiEditorIndexedSymbol *symbols,
                          size_t count,
                          const char *parent_id)
{
    size_t index;
    size_t children = 0U;
    for (index = 0U; index < count; ++index) {
        if (strcmp(symbols[index].parent_id, parent_id) == 0) children += 1U;
    }
    return children;
}

static size_t next_child(const UmiEditorIndexedSymbol *symbols,
                         const unsigned char *emitted,
                         size_t count,
                         const char *parent_id)
{
    size_t index;
    size_t best = SIZE_MAX;
    for (index = 0U; index < count; ++index) {
        if (emitted[index] ||
            strcmp(symbols[index].parent_id, parent_id) != 0) continue;
        if (best == SIZE_MAX || umi_editor_source_location_compare(
                &symbols[index].location, &symbols[best].location) < 0) {
            best = index;
        }
    }
    return best;
}

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

    if (depth > symbol_count || emitted[symbol_index]) return UMI_STATUS_OK;
    status = reserve_entries(outline, outline->count + 1U);
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

    for (;;) {
        child = next_child(symbols,
                           emitted,
                           symbol_count,
                           symbols[symbol_index].id);
        if (child == SIZE_MAX) break;
        status = append_hierarchy(outline,
                                  symbols,
                                  emitted,
                                  symbol_count,
                                  child,
                                  depth + 1U);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

static size_t entry_index(const UmiEditorDocumentOutline *outline,
                          const char *symbol_id)
{
    size_t index;
    for (index = 0U; index < outline->count; ++index) {
        if (strcmp(outline->entries[index].symbol_id, symbol_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

static void refresh_visibility(UmiEditorDocumentOutline *outline)
{
    size_t index;
    for (index = 0U; index < outline->count; ++index) {
        size_t parent;
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

UmiStatus umi_editor_document_outline_create(
    UmiEditorDocumentOutline **out_outline)
{
    UmiEditorDocumentOutline *outline;
    if (out_outline == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_outline = NULL;
    outline = (UmiEditorDocumentOutline *)calloc(1U, sizeof(*outline));
    if (outline == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    outline->revision = 1U;
    *out_outline = outline;
    return UMI_STATUS_OK;
}

void umi_editor_document_outline_destroy(UmiEditorDocumentOutline *outline)
{
    if (outline == NULL) return;
    free(outline->entries);
    outline->entries = NULL;
    free(outline);
}

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

    if (outline == NULL || symbol_index == NULL || document_id == NULL ||
        document_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    source_count = umi_editor_symbol_index_count(symbol_index);
    symbols = (UmiEditorIndexedSymbol *)calloc(
        source_count > 0U ? source_count : 1U, sizeof(*symbols));
    emitted = (unsigned char *)calloc(source_count > 0U ? source_count : 1U,
                                      sizeof(*emitted));
    if (symbols == NULL || emitted == NULL) {
        free(symbols);
        free(emitted);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (index = 0U; index < source_count; ++index) {
        UmiEditorIndexedSymbol symbol;
        status = umi_editor_symbol_index_at(symbol_index, index, &symbol);
        if (status != UMI_STATUS_OK) break;
        if (strcmp(symbol.document_id, document_id) == 0) {
            symbols[symbol_count++] = symbol;
        }
    }
    outline->count = 0U;
    if (status == UMI_STATUS_OK) {
        if (symbol_count > 1U) {
            qsort(symbols,
                  symbol_count,
                  sizeof(*symbols),
                  compare_symbols);
        }
        for (index = 0U; index < symbol_count; ++index) {
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
                if (status != UMI_STATUS_OK) break;
            }
        }
    }
    if (status == UMI_STATUS_OK) {
        for (index = 0U; index < symbol_count; ++index) {
            if (!emitted[index]) {
                status = append_hierarchy(outline,
                                          symbols,
                                          emitted,
                                          symbol_count,
                                          index,
                                          0U);
                if (status != UMI_STATUS_OK) break;
            }
        }
    }
    free(symbols);
    free(emitted);
    if (status == UMI_STATUS_OK) {
        refresh_visibility(outline);
        outline->revision = next_revision(outline->revision);
    }
    return status;
}

UmiStatus umi_editor_document_outline_select(
    UmiEditorDocumentOutline *outline,
    const char *symbol_id)
{
    size_t selected;
    size_t index;
    if (outline == NULL || symbol_id == NULL || symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    selected = entry_index(outline, symbol_id);
    if (selected == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < outline->count; ++index) {
        outline->entries[index].selected = index == selected;
    }
    outline->revision = next_revision(outline->revision);
    return UMI_STATUS_OK;
}

static int contains(const UmiEditorSourceLocation *range,
                    const UmiEditorSourceLocation *point)
{
    if (strcmp(range->uri, point->uri) != 0 ||
        point->line < range->line || point->line > range->end_line) return 0;
    if (point->line == range->line && point->column < range->column) return 0;
    if (point->line == range->end_line && point->column > range->end_column) {
        return 0;
    }
    return 1;
}

UmiStatus umi_editor_document_outline_select_location(
    UmiEditorDocumentOutline *outline,
    const UmiEditorSourceLocation *location)
{
    size_t best = SIZE_MAX;
    size_t best_depth = 0U;
    size_t index;

    if (outline == NULL ||
        umi_editor_source_location_validate(location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < outline->count; ++index) {
        if (contains(&outline->entries[index].range, location) &&
            (best == SIZE_MAX || outline->entries[index].depth >= best_depth)) {
            best = index;
            best_depth = outline->entries[index].depth;
        }
    }
    if (best == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return umi_editor_document_outline_select(outline,
                                               outline->entries[best].symbol_id);
}

UmiStatus umi_editor_document_outline_set_expanded(
    UmiEditorDocumentOutline *outline,
    const char *symbol_id,
    int expanded)
{
    size_t index;
    if (outline == NULL || symbol_id == NULL || symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = entry_index(outline, symbol_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    outline->entries[index].expanded = expanded != 0;
    refresh_visibility(outline);
    outline->revision = next_revision(outline->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_document_outline_entry_at(
    const UmiEditorDocumentOutline *outline,
    size_t index,
    UmiEditorDocumentOutlineEntry *out_entry)
{
    if (outline == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= outline->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = outline->entries[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_document_outline_visible_entry_at(
    const UmiEditorDocumentOutline *outline,
    size_t visible_index,
    UmiEditorDocumentOutlineEntry *out_entry)
{
    size_t index;
    size_t current = 0U;
    if (outline == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < outline->count; ++index) {
        if (!outline->entries[index].visible) continue;
        if (current == visible_index) {
            *out_entry = outline->entries[index];
            return UMI_STATUS_OK;
        }
        current += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

size_t umi_editor_document_outline_count(
    const UmiEditorDocumentOutline *outline)
{
    return outline != NULL ? outline->count : 0U;
}

size_t umi_editor_document_outline_visible_count(
    const UmiEditorDocumentOutline *outline)
{
    size_t count = 0U;
    size_t index;
    if (outline == NULL) return 0U;
    for (index = 0U; index < outline->count; ++index) {
        if (outline->entries[index].visible) count += 1U;
    }
    return count;
}

uint64_t umi_editor_document_outline_revision(
    const UmiEditorDocumentOutline *outline)
{
    return outline != NULL ? outline->revision : 0U;
}
