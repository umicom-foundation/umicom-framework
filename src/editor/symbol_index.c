/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/symbol_index.c
 *
 * PURPOSE:
 *   Implement dynamic workspace symbol storage, fuzzy search, child lookup and
 *   smallest-enclosing-symbol resolution with deterministic ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/symbol_index.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorSymbolIndex {
    UmiEditorIndexedSymbol *items;
    size_t count;
    size_t capacity;
    uint64_t next_sequence;
    uint64_t revision;
};

typedef struct SymbolKindName {
    UmiEditorIndexedSymbolKind kind;
    const char *name;
} SymbolKindName;

static const SymbolKindName SYMBOL_KIND_NAMES[] = {
    { UMI_EDITOR_INDEXED_SYMBOL_UNKNOWN, "unknown" },
    { UMI_EDITOR_INDEXED_SYMBOL_FILE, "file" },
    { UMI_EDITOR_INDEXED_SYMBOL_MODULE, "module" },
    { UMI_EDITOR_INDEXED_SYMBOL_NAMESPACE, "namespace" },
    { UMI_EDITOR_INDEXED_SYMBOL_PACKAGE, "package" },
    { UMI_EDITOR_INDEXED_SYMBOL_CLASS, "class" },
    { UMI_EDITOR_INDEXED_SYMBOL_METHOD, "method" },
    { UMI_EDITOR_INDEXED_SYMBOL_PROPERTY, "property" },
    { UMI_EDITOR_INDEXED_SYMBOL_FIELD, "field" },
    { UMI_EDITOR_INDEXED_SYMBOL_CONSTRUCTOR, "constructor" },
    { UMI_EDITOR_INDEXED_SYMBOL_ENUM, "enum" },
    { UMI_EDITOR_INDEXED_SYMBOL_INTERFACE, "interface" },
    { UMI_EDITOR_INDEXED_SYMBOL_FUNCTION, "function" },
    { UMI_EDITOR_INDEXED_SYMBOL_VARIABLE, "variable" },
    { UMI_EDITOR_INDEXED_SYMBOL_CONSTANT, "constant" },
    { UMI_EDITOR_INDEXED_SYMBOL_STRING, "string" },
    { UMI_EDITOR_INDEXED_SYMBOL_NUMBER, "number" },
    { UMI_EDITOR_INDEXED_SYMBOL_BOOLEAN, "boolean" },
    { UMI_EDITOR_INDEXED_SYMBOL_ARRAY, "array" },
    { UMI_EDITOR_INDEXED_SYMBOL_OBJECT, "object" },
    { UMI_EDITOR_INDEXED_SYMBOL_KEY, "key" },
    { UMI_EDITOR_INDEXED_SYMBOL_NULL, "null" },
    { UMI_EDITOR_INDEXED_SYMBOL_ENUM_MEMBER, "enum-member" },
    { UMI_EDITOR_INDEXED_SYMBOL_STRUCTURE, "structure" },
    { UMI_EDITOR_INDEXED_SYMBOL_EVENT, "event" },
    { UMI_EDITOR_INDEXED_SYMBOL_OPERATOR, "operator" },
    { UMI_EDITOR_INDEXED_SYMBOL_TYPE_PARAMETER, "type-parameter" },
    { UMI_EDITOR_INDEXED_SYMBOL_MACRO, "macro" },
    { UMI_EDITOR_INDEXED_SYMBOL_LABEL, "label" }
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the validate symbol operation used by this module and its client applications. */
static UmiStatus validate_symbol(const UmiEditorIndexedSymbol *symbol)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (symbol == NULL ||
        symbol->struct_size != (uint32_t)sizeof(*symbol) ||
        symbol->api_version != UMI_EDITOR_SYMBOL_INDEX_API_VERSION ||
        !terminated(symbol->id, sizeof(symbol->id)) || symbol->id[0] == '\0' ||
        !terminated(symbol->parent_id, sizeof(symbol->parent_id)) ||
        !terminated(symbol->provider_id, sizeof(symbol->provider_id)) ||
        !terminated(symbol->workspace_id, sizeof(symbol->workspace_id)) ||
        !terminated(symbol->document_id, sizeof(symbol->document_id)) ||
        symbol->document_id[0] == '\0' ||
        !terminated(symbol->language_id, sizeof(symbol->language_id)) ||
        !terminated(symbol->name, sizeof(symbol->name)) ||
        symbol->name[0] == '\0' ||
        !terminated(symbol->qualified_name, sizeof(symbol->qualified_name)) ||
        !terminated(symbol->container_name, sizeof(symbol->container_name)) ||
        !terminated(symbol->signature, sizeof(symbol->signature)) ||
        !terminated(symbol->detail, sizeof(symbol->detail)) ||
        symbol->kind < UMI_EDITOR_INDEXED_SYMBOL_UNKNOWN ||
        symbol->kind > UMI_EDITOR_INDEXED_SYMBOL_LABEL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_editor_source_location_validate(&symbol->location);
}

/* Provide the reserve symbols operation used by this module and its client applications. */
static UmiStatus reserve_symbols(UmiEditorSymbolIndex *index, size_t required)
{
    size_t capacity;
    UmiEditorIndexedSymbol *replacement;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= index->capacity) return UMI_STATUS_OK;
    capacity = index->capacity > 0U
        ? index->capacity
        : UMI_EDITOR_SYMBOL_INDEX_DEFAULT_CAPACITY;
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
    replacement = (UmiEditorIndexedSymbol *)realloc(
        index->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    index->items = replacement;
    index->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiEditorSymbolIndex *index,
                         const char *symbol_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || symbol_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < index->count; ++position) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(index->items[position].id, symbol_id) == 0) return position;
    }
    return SIZE_MAX;
}

/*
 * Initialise editor symbol index from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_editor_symbol_index_create(size_t initial_capacity,
                                          UmiEditorSymbolIndex **out_index)
{
    UmiEditorSymbolIndex *index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_index = NULL;
    /* Apply this branch only when its contract condition is satisfied. */
    if (initial_capacity == 0U) {
        initial_capacity = UMI_EDITOR_SYMBOL_INDEX_DEFAULT_CAPACITY;
    }
    index = (UmiEditorSymbolIndex *)calloc(1U, sizeof(*index));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = reserve_symbols(index, initial_capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(index);
        return status;
    }
    index->next_sequence = 1U;
    index->revision = 1U;
    *out_index = index;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor symbol index so the same storage can be reused
 * safely.
 */
void umi_editor_symbol_index_destroy(UmiEditorSymbolIndex *index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL) return;
    free(index->items);
    index->items = NULL;
    free(index);
}

/*
 * Provide the editor symbol index upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_symbol_index_upsert(UmiEditorSymbolIndex *index,
                                          const UmiEditorIndexedSymbol *symbol)
{
    UmiEditorIndexedSymbol stored;
    size_t position;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || validate_symbol(symbol) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(index, symbol->id);
    stored = *symbol;
    /* Apply this operation only while the related capability or state is available. */
    if (umi_editor_source_location_validate(&stored.selection_location) !=
        UMI_STATUS_OK) {
        stored.selection_location = stored.location;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (position == SIZE_MAX) {
        status = reserve_symbols(index, index->count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        position = index->count++;
        stored.sequence = index->next_sequence++;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index->next_sequence == 0U) index->next_sequence = 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        stored.sequence = index->items[position].sequence;
    }
    index->revision = next_revision(index->revision);
    stored.revision = index->revision;
    index->items[position] = stored;
    return UMI_STATUS_OK;
}

/*
 * Remove editor symbol index while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_symbol_index_remove(UmiEditorSymbolIndex *index,
                                          const char *symbol_id)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || symbol_id == NULL || symbol_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(index, symbol_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position + 1U < index->count) {
        (void)memmove(index->items + position,
                      index->items + position + 1U,
                      (index->count - position - 1U) * sizeof(*index->items));
    }
    index->count -= 1U;
    index->revision = next_revision(index->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol index remove document operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_index_remove_document(UmiEditorSymbolIndex *index,
                                                   const char *document_id,
                                                   size_t *out_removed_count)
{
    size_t read_position;
    size_t write_position = 0U;
    size_t removed = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || document_id == NULL || document_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (read_position = 0U; read_position < index->count; ++read_position) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(index->items[read_position].document_id, document_id) == 0) {
            removed += 1U;
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (write_position != read_position) {
            index->items[write_position] = index->items[read_position];
        }
        write_position += 1U;
    }
    index->count = write_position;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (removed > 0U) index->revision = next_revision(index->revision);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_removed_count != NULL) *out_removed_count = removed;
    return removed > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

/*
 * Find editor symbol index while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_editor_symbol_index_find(const UmiEditorSymbolIndex *index,
                                        const char *symbol_id,
                                        UmiEditorIndexedSymbol *out_symbol)
{
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || symbol_id == NULL || out_symbol == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    position = find_index(index, symbol_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (position == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_symbol = index->items[position];
    return UMI_STATUS_OK;
}

/*
 * Find editor symbol index while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_editor_symbol_index_at(const UmiEditorSymbolIndex *index,
                                      size_t position,
                                      UmiEditorIndexedSymbol *out_symbol)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || out_symbol == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= index->count) return UMI_STATUS_NOT_FOUND;
    *out_symbol = index->items[position];
    return UMI_STATUS_OK;
}

/* Provide the location contains operation used by this module and its client applications. */
static int location_contains(const UmiEditorSourceLocation *range,
                             const UmiEditorSourceLocation *point)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(range->uri, point->uri) != 0) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (point->line < range->line || point->line > range->end_line) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (point->line == range->line && point->column < range->column) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (point->line == range->end_line && point->column > range->end_column) {
        return 0;
    }
    return 1;
}

/*
 * Provide the editor symbol index find enclosing operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_index_find_enclosing(
    const UmiEditorSymbolIndex *index,
    const UmiEditorSourceLocation *location,
    UmiEditorIndexedSymbol *out_symbol)
{
    const UmiEditorIndexedSymbol *best = NULL;
    uint64_t best_line_span = UINT64_MAX;
    uint64_t best_column_span = UINT64_MAX;
    size_t position;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || out_symbol == NULL ||
        umi_editor_source_location_validate(location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < index->count; ++position) {
        const UmiEditorSourceLocation *range = &index->items[position].location;
        uint64_t line_span;
        uint64_t column_span;

        /* Apply this branch only when its contract condition is satisfied. */
        if (!location_contains(range, location)) continue;
        line_span = range->end_line - range->line;
        column_span = line_span == 0U
            ? range->end_column - range->column
            : range->end_column;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (best == NULL || line_span < best_line_span ||
            (line_span == best_line_span && column_span < best_column_span)) {
            best = &index->items[position];
            best_line_span = line_span;
            best_column_span = column_span;
        }
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (best == NULL) return UMI_STATUS_NOT_FOUND;
    *out_symbol = *best;
    return UMI_STATUS_OK;
}

/* Provide the same character operation used by this module and its client applications. */
static int same_character(char left, char right, int case_sensitive)
{
    unsigned char left_byte = (unsigned char)left;
    unsigned char right_byte = (unsigned char)right;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!case_sensitive) {
        left_byte = (unsigned char)tolower(left_byte);
        right_byte = (unsigned char)tolower(right_byte);
    }
    return left_byte == right_byte;
}

/* Provide the text equal operation used by this module and its client applications. */
static int text_equal(const char *left, const char *right, int case_sensitive)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*left != '\0' && *right != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (!same_character(*left, *right, case_sensitive)) return 0;
        left += 1;
        right += 1;
    }
    return *left == *right;
}

/* Provide the text prefix operation used by this module and its client applications. */
static int text_prefix(const char *text, const char *prefix, int case_sensitive)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*prefix != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*text == '\0' || !same_character(*text, *prefix, case_sensitive)) {
            return 0;
        }
        text += 1;
        prefix += 1;
    }
    return 1;
}

/* Provide the text substring operation used by this module and its client applications. */
static const char *text_substring(const char *text,
                                  const char *needle,
                                  int case_sensitive)
{
    const char *cursor;

    /* Apply this branch only when its contract condition is satisfied. */
    if (needle[0] == '\0') return text;
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = text; cursor[0] != '\0'; ++cursor) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (text_prefix(cursor, needle, case_sensitive)) return cursor;
    }
    return NULL;
}

/* Provide the fuzzy score operation used by this module and its client applications. */
static int32_t fuzzy_score(const char *candidate,
                           const char *query,
                           int case_sensitive,
                           size_t *out_matched)
{
    size_t query_count = strlen(query);
    size_t matched = 0U;
    size_t gaps = 0U;
    const char *cursor;
    const char *substring;

    *out_matched = 0U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (query_count == 0U) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (text_equal(candidate, query, case_sensitive)) {
        *out_matched = query_count;
        return 10000;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (text_prefix(candidate, query, case_sensitive)) {
        *out_matched = query_count;
        return 8000 - (int32_t)(strlen(candidate) - query_count);
    }
    substring = text_substring(candidate, query, case_sensitive);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (substring != NULL) {
        *out_matched = query_count;
        return 6000 - (int32_t)(substring - candidate);
    }
    cursor = candidate;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0' && matched < query_count) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (same_character(*cursor, query[matched], case_sensitive)) {
            matched += 1U;
        } else /* Use the stable identifier comparison to choose the matching record or policy. */ if (matched > 0U) {
            gaps += 1U;
        }
        cursor += 1;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (matched != query_count) return 0;
    *out_matched = matched;
    return 3000 - (int32_t)(gaps > 2000U ? 2000U : gaps);
}

/* Provide the match order operation used by this module and its client applications. */
static int match_order(const UmiEditorSymbolMatch *left,
                       const UmiEditorSymbolMatch *right)
{
    int location_order;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->score > right->score) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->score < right->score) return 1;
    location_order = umi_editor_source_location_compare(&left->symbol.location,
                                                         &right->symbol.location);
    /* Apply this branch only when its contract condition is satisfied. */
    if (location_order != 0) return location_order;
    return strcmp(left->symbol.name, right->symbol.name);
}

/* Provide the insert match operation used by this module and its client applications. */
static void insert_match(UmiEditorSymbolMatch *matches,
                         size_t *count,
                         size_t capacity,
                         const UmiEditorSymbolMatch *match)
{
    size_t position = 0U;
    size_t move_count;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (position < *count && match_order(&matches[position], match) <= 0) {
        position += 1U;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= capacity) return;
    move_count = *count < capacity ? *count - position
                                   : capacity - position - 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (move_count > 0U) {
        (void)memmove(matches + position + 1U,
                      matches + position,
                      move_count * sizeof(*matches));
    }
    matches[position] = *match;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (*count < capacity) *count += 1U;
}

/*
 * Provide the editor symbol index search operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_symbol_index_search(
    const UmiEditorSymbolIndex *index,
    const UmiEditorSymbolQuery *query,
    UmiEditorSymbolMatch *out_matches,
    size_t match_capacity,
    size_t *out_match_count)
{
    size_t position;
    size_t result_count = 0U;
    size_t limit;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || query == NULL || out_match_count == NULL ||
        query->struct_size != (uint32_t)sizeof(*query) ||
        query->api_version != UMI_EDITOR_SYMBOL_INDEX_API_VERSION ||
        (out_matches == NULL && match_capacity > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    limit = query->maximum_results > 0U &&
            query->maximum_results < match_capacity
        ? query->maximum_results
        : match_capacity;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < index->count; ++position) {
        const UmiEditorIndexedSymbol *symbol = &index->items[position];
        UmiEditorSymbolMatch match = {0};
        int32_t name_score;
        int32_t qualified_score;
        size_t name_matched;
        size_t qualified_matched;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (query->workspace_id != NULL && query->workspace_id[0] != '\0' &&
            strcmp(symbol->workspace_id, query->workspace_id) != 0) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (query->document_id != NULL && query->document_id[0] != '\0' &&
            strcmp(symbol->document_id, query->document_id) != 0) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (query->language_id != NULL && query->language_id[0] != '\0' &&
            strcmp(symbol->language_id, query->language_id) != 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (query->kind != UMI_EDITOR_INDEXED_SYMBOL_UNKNOWN &&
            symbol->kind != query->kind) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if ((symbol->flags & query->required_flags) != query->required_flags) {
            continue;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (!query->include_external &&
            (symbol->flags & UMI_EDITOR_INDEXED_SYMBOL_FLAG_EXTERNAL) != 0U) {
            continue;
        }
        name_score = fuzzy_score(symbol->name,
                                 query->text != NULL ? query->text : "",
                                 query->case_sensitive,
                                 &name_matched);
        qualified_score = fuzzy_score(
            symbol->qualified_name,
            query->text != NULL ? query->text : "",
            query->case_sensitive,
            &qualified_matched);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (name_score <= 0 && qualified_score <= 0) continue;
        match.struct_size = (uint32_t)sizeof(match);
        match.api_version = UMI_EDITOR_SYMBOL_INDEX_API_VERSION;
        match.symbol = *symbol;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (name_score >= qualified_score) {
            match.score = name_score;
            match.matched_character_count = name_matched;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            match.score = qualified_score - 100;
            match.matched_character_count = qualified_matched;
        }
        insert_match(out_matches, &result_count, limit, &match);
    }
    *out_match_count = result_count;
    return UMI_STATUS_OK;
}

/*
 * Provide the symbol location order operation used by this module and its client
 * applications.
 */
static int symbol_location_order(const void *left_value, const void *right_value)
{
    const UmiEditorIndexedSymbol *left =
        (const UmiEditorIndexedSymbol *)left_value;
    const UmiEditorIndexedSymbol *right =
        (const UmiEditorIndexedSymbol *)right_value;
    int order = umi_editor_source_location_compare(&left->location,
                                                    &right->location);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    return strcmp(left->name, right->name);
}

/*
 * Provide the editor symbol index children operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_symbol_index_children(
    const UmiEditorSymbolIndex *index,
    const char *parent_id,
    const char *document_id,
    UmiEditorIndexedSymbol *out_symbols,
    size_t symbol_capacity,
    size_t *out_symbol_count)
{
    size_t position;
    size_t count = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (index == NULL || parent_id == NULL || out_symbol_count == NULL ||
        (out_symbols == NULL && symbol_capacity > 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < index->count; ++position) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(index->items[position].parent_id, parent_id) != 0) continue;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (document_id != NULL && document_id[0] != '\0' &&
            strcmp(index->items[position].document_id, document_id) != 0) {
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (count < symbol_capacity) out_symbols[count] = index->items[position];
        count += 1U;
    }
    *out_symbol_count = count < symbol_capacity ? count : symbol_capacity;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_symbols != NULL && *out_symbol_count > 1U) {
        qsort(out_symbols,
              *out_symbol_count,
              sizeof(*out_symbols),
              symbol_location_order);
    }
    return count > symbol_capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor symbol index without changing their
 * state.
 */
size_t umi_editor_symbol_index_count(const UmiEditorSymbolIndex *index)
{
    return index != NULL ? index->count : 0U;
}

/*
 * Provide the editor symbol index revision operation used by this module and its client
 * applications.
 */
uint64_t umi_editor_symbol_index_revision(const UmiEditorSymbolIndex *index)
{
    return index != NULL ? index->revision : 0U;
}

/*
 * Provide the editor indexed symbol kind name operation used by this module and its client
 * applications.
 */
const char *umi_editor_indexed_symbol_kind_name(UmiEditorIndexedSymbolKind kind)
{
    size_t position;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U;
         position < sizeof(SYMBOL_KIND_NAMES) / sizeof(SYMBOL_KIND_NAMES[0]);
         ++position) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (SYMBOL_KIND_NAMES[position].kind == kind) {
            return SYMBOL_KIND_NAMES[position].name;
        }
    }
    return "unknown";
}

/*
 * Provide the editor indexed symbol kind from name operation used by this module and its
 * client applications.
 */
UmiEditorIndexedSymbolKind umi_editor_indexed_symbol_kind_from_name(
    const char *name)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (name == NULL) return UMI_EDITOR_INDEXED_SYMBOL_UNKNOWN;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U;
         position < sizeof(SYMBOL_KIND_NAMES) / sizeof(SYMBOL_KIND_NAMES[0]);
         ++position) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (text_equal(SYMBOL_KIND_NAMES[position].name, name, 0)) {
            return SYMBOL_KIND_NAMES[position].kind;
        }
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (text_equal(name, "struct", 0)) return UMI_EDITOR_INDEXED_SYMBOL_STRUCTURE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (text_equal(name, "enum member", 0)) {
        return UMI_EDITOR_INDEXED_SYMBOL_ENUM_MEMBER;
    }
    return UMI_EDITOR_INDEXED_SYMBOL_UNKNOWN;
}
