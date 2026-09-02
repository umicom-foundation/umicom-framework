/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/symbol_navigation_session.c
 *
 * PURPOSE:
 *   Implement document/workspace symbol aggregation and navigation.  Ranking
 *   combines provider priority, exact/prefix/substring/subsequence matching and
 *   document locality.  Parent identifiers are retained so outline depth and a
 *   breadcrumb-compatible selected path can be reconstructed after sorting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/symbol_navigation_session.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorSymbolNavigationSession {
    UmiEditorNavigationProviderRegistry *registry;
    UmiEditorNavigationRequest request;
    UmiEditorSymbolNavigationScope scope;
    UmiEditorSymbolNavigationEntry *entries;
    size_t count;
    size_t capacity;
    size_t visible_count;
    size_t selected_index;
    size_t path_indices[UMI_EDITOR_SYMBOL_NAVIGATION_PATH_CAPACITY];
    size_t path_count;
    UmiEditorNavigationProviderReport *reports;
    size_t report_count;
    size_t report_capacity;
    uint64_t sequence;
    uint64_t revision;
    UmiEditorSymbolNavigationState state;
    int has_request;
    int cancel_requested;
    int follow_cursor;
    int truncated;
    char filter[UMI_EDITOR_SYMBOL_NAVIGATION_FILTER_CAPACITY];
};

typedef struct SymbolSinkContext {
    UmiEditorSymbolNavigationSession *session;
    const UmiEditorNavigationProviderRegistration *provider;
} SymbolSinkContext;

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/*
 * Provide the internal cancelled operation used by this module and its client
 * applications.
 */
static int internal_cancelled(void *user_data)
{
    const UmiEditorSymbolNavigationSession *session =
        (const UmiEditorSymbolNavigationSession *)user_data;
    return session != NULL && session->cancel_requested;
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

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the valid location operation used by this module and its client applications. */
static int valid_location(const UmiEditorSourceLocation *location)
{
    return umi_editor_source_location_validate(location) == UMI_STATUS_OK &&
           terminated(location->uri, sizeof(location->uri)) &&
           terminated(location->label, sizeof(location->label)) &&
           terminated(location->symbol_id, sizeof(location->symbol_id)) &&
           terminated(location->preview, sizeof(location->preview));
}

/* Provide the valid symbol operation used by this module and its client applications. */
static int valid_symbol(const UmiEditorIndexedSymbol *symbol)
{
    return symbol != NULL &&
           symbol->struct_size == (uint32_t)sizeof(*symbol) &&
           symbol->api_version == UMI_EDITOR_SYMBOL_INDEX_API_VERSION &&
           terminated(symbol->id, sizeof(symbol->id)) &&
           terminated(symbol->parent_id, sizeof(symbol->parent_id)) &&
           terminated(symbol->provider_id, sizeof(symbol->provider_id)) &&
           terminated(symbol->workspace_id, sizeof(symbol->workspace_id)) &&
           terminated(symbol->document_id, sizeof(symbol->document_id)) &&
           terminated(symbol->language_id, sizeof(symbol->language_id)) &&
           terminated(symbol->name, sizeof(symbol->name)) &&
           terminated(symbol->qualified_name, sizeof(symbol->qualified_name)) &&
           terminated(symbol->container_name, sizeof(symbol->container_name)) &&
           terminated(symbol->detail, sizeof(symbol->detail)) &&
           terminated(symbol->signature, sizeof(symbol->signature)) &&
           symbol->id[0] != '\0' && symbol->name[0] != '\0' &&
           valid_location(&symbol->location) &&
           (symbol->selection_location.uri[0] == '\0' ||
            valid_location(&symbol->selection_location));
}

/* Provide the ascii equal operation used by this module and its client applications. */
static int ascii_equal(const char *left, const char *right)
{
    size_t index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL || right == NULL) return 0;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (left[index] != '\0' && right[index] != '\0') {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (tolower((unsigned char)left[index]) !=
            tolower((unsigned char)right[index])) {
            return 0;
        }
        ++index;
    }
    return left[index] == '\0' && right[index] == '\0';
}

/* Provide the ascii prefix operation used by this module and its client applications. */
static int ascii_prefix(const char *text, const char *prefix)
{
    size_t index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL || prefix == NULL) return 0;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (prefix[index] != '\0') {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (text[index] == '\0' ||
            tolower((unsigned char)text[index]) !=
                tolower((unsigned char)prefix[index])) {
            return 0;
        }
        ++index;
    }
    return 1;
}

/* Provide the ascii substring operation used by this module and its client applications. */
static int ascii_substring(const char *text, const char *pattern)
{
    size_t start;
    size_t text_length;
    size_t pattern_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pattern == NULL || pattern[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    text_length = strlen(text);
    pattern_length = strlen(pattern);
    /* Apply this branch only when its contract condition is satisfied. */
    if (pattern_length > text_length) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (start = 0U; start + pattern_length <= text_length; ++start) {
        size_t offset;
        int match = 1;
        /* Visit each bounded item once so every record receives the same rule. */
        for (offset = 0U; offset < pattern_length; ++offset) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (tolower((unsigned char)text[start + offset]) !=
                tolower((unsigned char)pattern[offset])) {
                match = 0;
                break;
            }
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (match) return 1;
    }
    return 0;
}

/* Provide the ascii subsequence operation used by this module and its client applications. */
static int ascii_subsequence(const char *text, const char *pattern)
{
    size_t text_index = 0U;
    size_t pattern_index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pattern == NULL || pattern[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (text[text_index] != '\0' && pattern[pattern_index] != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (tolower((unsigned char)text[text_index]) ==
            tolower((unsigned char)pattern[pattern_index])) {
            ++pattern_index;
        }
        ++text_index;
    }
    return pattern[pattern_index] == '\0';
}

/* Provide the bounded score operation used by this module and its client applications. */
static int32_t bounded_score(int64_t score)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (score > INT32_MAX) return INT32_MAX;
    /* Apply this branch only when its contract condition is satisfied. */
    if (score < INT32_MIN) return INT32_MIN;
    return (int32_t)score;
}

/* Provide the match score operation used by this module and its client applications. */
static int32_t match_score(const UmiEditorIndexedSymbol *symbol,
                           const char *filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL || filter[0] == '\0') return 100;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (ascii_equal(symbol->name, filter) ||
        ascii_equal(symbol->qualified_name, filter)) {
        return 4000;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (ascii_prefix(symbol->name, filter) ||
        ascii_prefix(symbol->qualified_name, filter)) {
        return 3000;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (ascii_substring(symbol->name, filter) ||
        ascii_substring(symbol->qualified_name, filter) ||
        ascii_substring(symbol->detail, filter) ||
        ascii_substring(symbol->signature, filter)) {
        return 2000;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (ascii_subsequence(symbol->name, filter) ||
        ascii_subsequence(symbol->qualified_name, filter)) {
        return 1000;
    }
    return INT32_MIN;
}

/* Provide the symbol score operation used by this module and its client applications. */
static int32_t symbol_score(
    const UmiEditorSymbolNavigationSession *session,
    const UmiEditorIndexedSymbol *symbol,
    const char *filter,
    int32_t provider_priority)
{
    int32_t base_score;
    int64_t score;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || symbol == NULL) return INT32_MIN;
    base_score = match_score(symbol, filter);
    /* Apply this branch only when its contract condition is satisfied. */
    if (base_score == INT32_MIN) return INT32_MIN;
    score = (int64_t)base_score + (int64_t)provider_priority * 1000LL;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (session->request.document_id[0] != '\0' &&
        strcmp(symbol->document_id, session->request.document_id) == 0) {
        score += 500LL;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((symbol->flags & UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEFINITION) != 0U) {
        score += 100LL;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if ((symbol->flags & UMI_EDITOR_INDEXED_SYMBOL_FLAG_DEPRECATED) != 0U) {
        score -= 500LL;
    }
    return bounded_score(score);
}

/*
 * Provide the provider priority for operation used by this module and its client
 * applications.
 */
static int32_t provider_priority_for(
    const UmiEditorSymbolNavigationSession *session,
    const char *provider_id)
{
    UmiEditorNavigationProviderRegistration registration;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_editor_navigation_provider_registry_find(session->registry,
                                                       provider_id,
                                                       &registration) ==
        UMI_STATUS_OK) {
        return registration.descriptor.priority;
    }
    return 0;
}

/* Provide the reserve entries operation used by this module and its client applications. */
static UmiStatus reserve_entries(UmiEditorSymbolNavigationSession *session,
                                 size_t required)
{
    size_t capacity;
    UmiEditorSymbolNavigationEntry *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= session->capacity) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (required > UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS) {
        session->truncated = 1;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capacity = session->capacity > 0U ? session->capacity : 64U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS / 2U) {
            capacity = UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS;
            break;
        }
        capacity *= 2U;
    }
    replacement = (UmiEditorSymbolNavigationEntry *)realloc(
        session->entries, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->entries = replacement;
    session->capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the reserve reports operation used by this module and its client applications. */
static UmiStatus reserve_reports(UmiEditorSymbolNavigationSession *session,
                                 size_t required)
{
    size_t capacity;
    UmiEditorNavigationProviderReport *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= session->report_capacity) return UMI_STATUS_OK;
    /* Apply this branch only when its contract condition is satisfied. */
    if (required > UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    capacity = session->report_capacity > 0U ? session->report_capacity : 8U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (capacity < required) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (capacity > UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY / 2U) {
            capacity = UMI_EDITOR_NAVIGATION_PROVIDER_MAXIMUM_CAPACITY;
            break;
        }
        capacity *= 2U;
    }
    replacement = (UmiEditorNavigationProviderReport *)realloc(
        session->reports, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->reports = replacement;
    session->report_capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find symbol operation used by this module and its client applications. */
static size_t find_symbol(const UmiEditorSymbolNavigationSession *session,
                          const char *provider_id,
                          const char *symbol_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        const UmiEditorIndexedSymbol *symbol = &session->entries[index].symbol;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(symbol->id, symbol_id) == 0 &&
            strcmp(symbol->provider_id, provider_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Provide the same symbol location operation used by this module and its client
 * applications.
 */
static int same_symbol_location(const UmiEditorIndexedSymbol *left,
                                const UmiEditorIndexedSymbol *right)
{
    return strcmp(left->name, right->name) == 0 &&
           strcmp(left->location.uri, right->location.uri) == 0 &&
           left->location.line == right->location.line &&
           left->location.column == right->location.column;
}

/* Provide the find parent operation used by this module and its client applications. */
static size_t find_parent(const UmiEditorSymbolNavigationSession *session,
                          const UmiEditorIndexedSymbol *symbol)
{
    size_t index;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (symbol->parent_id[0] == '\0') return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(session->entries[index].symbol.provider_id,
                   symbol->provider_id) == 0 &&
            strcmp(session->entries[index].symbol.id, symbol->parent_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/* Provide the calculate depth operation used by this module and its client applications. */
static size_t calculate_depth(const UmiEditorSymbolNavigationSession *session,
                              size_t index)
{
    size_t depth = 0U;
    size_t parent = find_parent(session, &session->entries[index].symbol);
    size_t guard = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (parent != SIZE_MAX && guard < session->count) {
        ++depth;
        parent = find_parent(session, &session->entries[parent].symbol);
        ++guard;
    }
    return depth;
}

/* Provide the accept symbol operation used by this module and its client applications. */
static UmiStatus accept_symbol(SymbolSinkContext *context,
                               const UmiEditorIndexedSymbol *source)
{
    UmiEditorSymbolNavigationSession *session;
    UmiEditorSymbolNavigationEntry entry;
    size_t index;
    int32_t base_score;
    int64_t score;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || source == NULL || context->session == NULL ||
        context->provider == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    session = context->session;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->cancel_requested) return UMI_STATUS_CANCELLED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!valid_symbol(source)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->scope == UMI_EDITOR_SYMBOL_NAVIGATION_DOCUMENT &&
        session->request.document_id[0] != '\0' &&
        strcmp(source->document_id, session->request.document_id) != 0) {
        return UMI_STATUS_OK;
    }
    (void)memset(&entry, 0, sizeof(entry));
    entry.struct_size = (uint32_t)sizeof(entry);
    entry.api_version = UMI_EDITOR_SYMBOL_NAVIGATION_SESSION_API_VERSION;
    entry.symbol = *source;
    status = copy_text(entry.symbol.provider_id,
                       sizeof(entry.symbol.provider_id),
                       context->provider->descriptor.id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    base_score = match_score(&entry.symbol,
                             session->filter[0] != '\0'
                                 ? session->filter
                                 : session->request.text);
    entry.visible = base_score != INT32_MIN;
    score = symbol_score(session, &entry.symbol,
                         session->filter[0] != '\0'
                             ? session->filter
                             : session->request.text,
                         context->provider->descriptor.priority);
    entry.score = bounded_score(score);
    entry.symbol.sequence = ++session->sequence;
    index = find_symbol(session, entry.symbol.provider_id, entry.symbol.id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index != SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (entry.score > session->entries[index].score) {
            session->entries[index] = entry;
        }
        return UMI_STATUS_OK;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (same_symbol_location(&session->entries[index].symbol,
                                 &entry.symbol)) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (entry.score > session->entries[index].score) {
                session->entries[index] = entry;
            }
            return UMI_STATUS_OK;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->count >= session->request.maximum_results ||
        session->count >= UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS) {
        session->truncated = 1;
        return UMI_STATUS_OK;
    }
    status = reserve_entries(session, session->count + 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CAPACITY_EXCEEDED) return UMI_STATUS_OK;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    session->entries[session->count++] = entry;
    return UMI_STATUS_OK;
}

/* Provide the symbol sink operation used by this module and its client applications. */
static UmiStatus symbol_sink(const UmiEditorIndexedSymbol *symbol,
                             void *user_data)
{
    return accept_symbol((SymbolSinkContext *)user_data, symbol);
}

/* Provide the compare entries operation used by this module and its client applications. */
static int compare_entries(const void *left_pointer, const void *right_pointer)
{
    const UmiEditorSymbolNavigationEntry *left =
        (const UmiEditorSymbolNavigationEntry *)left_pointer;
    const UmiEditorSymbolNavigationEntry *right =
        (const UmiEditorSymbolNavigationEntry *)right_pointer;
    int order;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->score > right->score) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->score < right->score) return 1;
    order = strcmp(left->symbol.name, right->symbol.name);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    order = strcmp(left->symbol.location.uri, right->symbol.location.uri);
    /* Apply this branch only when its contract condition is satisfied. */
    if (order != 0) return order;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->symbol.location.line < right->symbol.location.line) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (left->symbol.location.line > right->symbol.location.line) return 1;
    return strcmp(left->symbol.id, right->symbol.id);
}

/*
 * Provide the rebuild selected path operation used by this module and its client
 * applications.
 */
static void rebuild_selected_path(UmiEditorSymbolNavigationSession *session)
{
    size_t reverse[UMI_EDITOR_SYMBOL_NAVIGATION_PATH_CAPACITY];
    size_t reverse_count = 0U;
    size_t current = session->selected_index;
    size_t guard = 0U;
    session->path_count = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (current != SIZE_MAX &&
           reverse_count < UMI_EDITOR_SYMBOL_NAVIGATION_PATH_CAPACITY &&
           guard < session->count) {
        reverse[reverse_count++] = current;
        current = find_parent(session, &session->entries[current].symbol);
        ++guard;
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (reverse_count > 0U) {
        session->path_indices[session->path_count++] = reverse[--reverse_count];
    }
}

/*
 * Provide the recompute visibility operation used by this module and its client
 * applications.
 */
static void recompute_visibility(UmiEditorSymbolNavigationSession *session)
{
    char selected_provider[UMI_EDITOR_NAVIGATION_PROVIDER_ID_CAPACITY] = {0};
    char selected_symbol[UMI_EDITOR_SYMBOL_ID_CAPACITY] = {0};
    size_t index;
    size_t first_visible = SIZE_MAX;

    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index != SIZE_MAX &&
        session->selected_index < session->count) {
        (void)copy_text(selected_provider, sizeof(selected_provider),
                        session->entries[session->selected_index].symbol.provider_id);
        (void)copy_text(selected_symbol, sizeof(selected_symbol),
                        session->entries[session->selected_index].symbol.id);
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        UmiEditorSymbolNavigationEntry *entry = &session->entries[index];
        entry->score = symbol_score(
            session, &entry->symbol, session->filter,
            provider_priority_for(session, entry->symbol.provider_id));
        entry->visible = entry->score != INT32_MIN;
        entry->selected = 0;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->count > 1U) {
        qsort(session->entries, session->count, sizeof(*session->entries),
              compare_entries);
    }
    session->visible_count = 0U;
    session->selected_index = SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        UmiEditorSymbolNavigationEntry *entry = &session->entries[index];
        entry->depth = calculate_depth(session, index);
        /* Apply this operation only while the related capability or state is available. */
        if (entry->visible) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (first_visible == SIZE_MAX) first_visible = index;
            ++session->visible_count;
            /* Apply this branch only when its contract condition is satisfied. */
            if (selected_symbol[0] != '\0' &&
                strcmp(entry->symbol.provider_id, selected_provider) == 0 &&
                strcmp(entry->symbol.id, selected_symbol) == 0) {
                session->selected_index = index;
            }
        }
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index == SIZE_MAX) {
        session->selected_index = first_visible;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index != SIZE_MAX) {
        session->entries[session->selected_index].selected = 1;
    }
    rebuild_selected_path(session);
}

/* Provide the initialise report operation used by this module and its client applications. */
static void initialise_report(UmiEditorNavigationProviderReport *report,
                              const char *provider_id,
                              uint64_t sequence)
{
    (void)memset(report, 0, sizeof(*report));
    report->struct_size = (uint32_t)sizeof(*report);
    report->api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    report->status = UMI_STATUS_OK;
    report->sequence = sequence;
    (void)copy_text(report->provider_id, sizeof(report->provider_id), provider_id);
}

/*
 * Initialise editor symbol navigation session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_symbol_navigation_session_create(
    UmiEditorNavigationProviderRegistry *registry,
    UmiEditorSymbolNavigationSession **out_session)
{
    UmiEditorSymbolNavigationSession *session;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_session == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_session = NULL;
    session = (UmiEditorSymbolNavigationSession *)calloc(1U, sizeof(*session));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->registry = registry;
    session->selected_index = SIZE_MAX;
    session->follow_cursor = 1;
    session->state = UMI_EDITOR_SYMBOL_NAVIGATION_CLOSED;
    session->revision = 1U;
    *out_session = session;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor symbol navigation session so the same storage can
 * be reused safely.
 */
void umi_editor_symbol_navigation_session_destroy(
    UmiEditorSymbolNavigationSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return;
    free(session->entries);
    free(session->reports);
    session->entries = NULL;
    session->reports = NULL;
    free(session);
}

/*
 * Provide the editor symbol navigation session open operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_open(
    UmiEditorSymbolNavigationSession *session,
    UmiEditorSymbolNavigationScope scope,
    const UmiEditorNavigationRequest *request)
{
    char preserved_filter[UMI_EDITOR_SYMBOL_NAVIGATION_FILTER_CAPACITY] = {0};
    UmiEditorNavigationProviderRegistration registration;
    UmiEditorNavigationCancellation cancellation;
    size_t index;
    size_t matched = 0U;
    UmiStatus first_failure = UMI_STATUS_NOT_FOUND;
    int preserve_filter;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || request == NULL ||
        (scope != UMI_EDITOR_SYMBOL_NAVIGATION_DOCUMENT &&
         scope != UMI_EDITOR_SYMBOL_NAVIGATION_WORKSPACE) ||
        request->struct_size != (uint32_t)sizeof(*request) ||
        request->api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
        request->request_id == 0U ||
        request->maximum_results > UMI_EDITOR_NAVIGATION_MAXIMUM_RESULTS ||
        (request->flags &
         ~(UmiEditorNavigationRequestFlags)UMI_EDITOR_NAVIGATION_REQUEST_ALL_FLAGS) != 0U ||
        !valid_location(&request->origin) ||
        !terminated(request->text, sizeof(request->text)) ||
        !terminated(request->workspace_id, sizeof(request->workspace_id)) ||
        !terminated(request->document_id, sizeof(request->document_id)) ||
        !terminated(request->language_id, sizeof(request->language_id)) ||
        !terminated(request->symbol_id, sizeof(request->symbol_id)) ||
        (scope == UMI_EDITOR_SYMBOL_NAVIGATION_DOCUMENT &&
         request->query_kind != UMI_EDITOR_NAVIGATION_QUERY_DOCUMENT_SYMBOL) ||
        (scope == UMI_EDITOR_SYMBOL_NAVIGATION_WORKSPACE &&
         request->query_kind != UMI_EDITOR_NAVIGATION_QUERY_WORKSPACE_SYMBOL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    preserve_filter = session->has_request &&
                      session->request.request_id == request->request_id;
    /* Apply this branch only when its contract condition is satisfied. */
    if (preserve_filter) {
        UmiStatus filter_status = copy_text(preserved_filter,
                                            sizeof(preserved_filter),
                                            session->filter);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (filter_status != UMI_STATUS_OK) return filter_status;
    }
    session->scope = scope;
    session->request = *request;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->request.maximum_results == 0U) {
        session->request.maximum_results =
            UMI_EDITOR_NAVIGATION_DEFAULT_MAXIMUM_RESULTS;
    }
    session->count = 0U;
    session->visible_count = 0U;
    session->report_count = 0U;
    session->selected_index = SIZE_MAX;
    session->path_count = 0U;
    session->cancel_requested = 0;
    session->truncated = 0;
    session->has_request = 1;
    session->state = UMI_EDITOR_SYMBOL_NAVIGATION_LOADING;
    {
        UmiStatus filter_status = copy_text(session->filter,
                                            sizeof(session->filter),
                                            preserve_filter
                                                ? preserved_filter
                                                : request->text);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (filter_status != UMI_STATUS_OK) {
            session->state = UMI_EDITOR_SYMBOL_NAVIGATION_FAILED;
            session->has_request = 0;
            return filter_status;
        }
    }
    cancellation.struct_size = (uint32_t)sizeof(cancellation);
    cancellation.api_version = UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION;
    cancellation.is_cancelled = internal_cancelled;
    cancellation.user_data = session;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_editor_navigation_provider_registry_count(session->registry);
         ++index) {
        UmiEditorNavigationProviderReport report;
        SymbolSinkContext context;
        UmiStatus status;
        size_t before_count;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_editor_navigation_provider_registry_at(session->registry, index,
                                                       &registration) !=
                UMI_STATUS_OK ||
            !umi_editor_navigation_provider_supports_request(&registration,
                                                             &session->request)) {
            continue;
        }
        ++matched;
        status = reserve_reports(session, session->report_count + 1U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        initialise_report(&report, registration.descriptor.id,
                          ++session->sequence);
        context.session = session;
        context.provider = &registration;
        before_count = session->count;
        status = registration.functions.symbols(
            registration.instance, &session->request, &cancellation, symbol_sink,
            &context, &report);
        /* Apply this branch only when its contract condition is satisfied. */
        if (report.struct_size != (uint32_t)sizeof(report) ||
            report.api_version != UMI_EDITOR_NAVIGATION_PROVIDER_API_VERSION ||
            !terminated(report.message, sizeof(report.message))) {
            initialise_report(&report, registration.descriptor.id,
                              session->sequence);
        }
        (void)copy_text(report.provider_id, sizeof(report.provider_id),
                        registration.descriptor.id);
        report.status = status;
        report.emitted_count = session->count - before_count;
        report.cancelled = session->cancel_requested || status == UMI_STATUS_CANCELLED;
        report.truncated = session->truncated;
        session->reports[session->report_count++] = report;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK && status != UMI_STATUS_NOT_FOUND &&
            status != UMI_STATUS_CANCELLED &&
            first_failure == UMI_STATUS_NOT_FOUND) {
            first_failure = status;
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_CANCELLED || session->cancel_requested) break;
    }
    recompute_visibility(session);
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->cancel_requested) {
        session->state = UMI_EDITOR_SYMBOL_NAVIGATION_CANCELLED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_CANCELLED;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (matched == 0U) {
        session->state = UMI_EDITOR_SYMBOL_NAVIGATION_FAILED;
        session->revision = next_revision(session->revision);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (session->count == 0U && first_failure != UMI_STATUS_NOT_FOUND) {
        session->state = UMI_EDITOR_SYMBOL_NAVIGATION_FAILED;
        session->revision = next_revision(session->revision);
        return first_failure;
    }
    session->state = UMI_EDITOR_SYMBOL_NAVIGATION_OPEN;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol navigation session refresh operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_refresh(
    UmiEditorSymbolNavigationSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!session->has_request) return UMI_STATUS_INVALID_STATE;
    return umi_editor_symbol_navigation_session_open(session, session->scope,
                                                     &session->request);
}

/*
 * Provide the editor symbol navigation session cancel operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_cancel(
    UmiEditorSymbolNavigationSession *session)
{
    UmiEditorNavigationProviderRegistration registration;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->state != UMI_EDITOR_SYMBOL_NAVIGATION_LOADING) {
        return UMI_STATUS_INVALID_STATE;
    }
    session->cancel_requested = 1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_editor_navigation_provider_registry_count(session->registry);
         ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_editor_navigation_provider_registry_at(session->registry, index,
                                                       &registration) ==
                UMI_STATUS_OK &&
            umi_editor_navigation_provider_supports_request(
                &registration, &session->request) &&
            registration.functions.cancel != NULL) {
            (void)registration.functions.cancel(registration.instance,
                                                session->request.request_id);
        }
    }
    session->state = UMI_EDITOR_SYMBOL_NAVIGATION_CANCELLED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol navigation session close operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_close(
    UmiEditorSymbolNavigationSession *session)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session->count = 0U;
    session->visible_count = 0U;
    session->report_count = 0U;
    session->selected_index = SIZE_MAX;
    session->path_count = 0U;
    session->has_request = 0;
    session->filter[0] = '\0';
    session->state = UMI_EDITOR_SYMBOL_NAVIGATION_CLOSED;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol navigation session set filter operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_set_filter(
    UmiEditorSymbolNavigationSession *session,
    const char *filter)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || filter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(session->filter, sizeof(session->filter), filter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    recompute_visibility(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol navigation session set follow cursor operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_set_follow_cursor(
    UmiEditorSymbolNavigationSession *session,
    int follow_cursor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    session->follow_cursor = follow_cursor != 0;
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol navigation session select operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_select(
    UmiEditorSymbolNavigationSession *session,
    size_t symbol_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (symbol_index >= session->count ||
        !session->entries[symbol_index].visible) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index != SIZE_MAX &&
        session->selected_index < session->count) {
        session->entries[session->selected_index].selected = 0;
    }
    session->selected_index = symbol_index;
    session->entries[symbol_index].selected = 1;
    rebuild_selected_path(session);
    session->revision = next_revision(session->revision);
    return UMI_STATUS_OK;
}

/* Provide the select relative operation used by this module and its client applications. */
static UmiStatus select_relative(UmiEditorSymbolNavigationSession *session,
                                 int direction,
                                 int wrap)
{
    size_t index;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (session->visible_count == 0U) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index == SIZE_MAX) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < session->count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (session->entries[index].visible) {
                return umi_editor_symbol_navigation_session_select(session,
                                                                   index);
            }
        }
    } else /* Apply this branch only when its contract condition is satisfied. */ if (direction > 0) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = session->selected_index + 1U; index < session->count;
             ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (session->entries[index].visible) {
                return umi_editor_symbol_navigation_session_select(session,
                                                                   index);
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (wrap) {
            /* Visit each bounded item once so every record receives the same rule. */
            for (index = 0U; index < session->selected_index; ++index) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (session->entries[index].visible) {
                    return umi_editor_symbol_navigation_session_select(session,
                                                                       index);
                }
            }
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        index = session->selected_index;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (index > 0U) {
            --index;
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (session->entries[index].visible) {
                return umi_editor_symbol_navigation_session_select(session,
                                                                   index);
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (wrap) {
            index = session->count;
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (index > session->selected_index + 1U) {
                --index;
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (session->entries[index].visible) {
                    return umi_editor_symbol_navigation_session_select(session,
                                                                       index);
                }
            }
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the editor symbol navigation session select next operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_select_next(
    UmiEditorSymbolNavigationSession *session,
    int wrap)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return select_relative(session, 1, wrap != 0);
}

/*
 * Provide the editor symbol navigation session select previous operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_select_previous(
    UmiEditorSymbolNavigationSession *session,
    int wrap)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return select_relative(session, -1, wrap != 0);
}

/* Provide the location contains operation used by this module and its client applications. */
static int location_contains(const UmiEditorSourceLocation *container,
                             const UmiEditorSourceLocation *location)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(container->uri, location->uri) != 0) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (container->end_byte_offset > container->byte_offset) {
        return location->byte_offset >= container->byte_offset &&
               location->byte_offset < container->end_byte_offset;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (container->line == container->end_line &&
        container->column == container->end_column) {
        return location->line == container->line &&
               location->column == container->column;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (location->line < container->line ||
        location->line > container->end_line) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (location->line == container->line &&
        location->column < container->column) {
        return 0;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (location->line == container->end_line &&
        location->column >= container->end_column) {
        return 0;
    }
    return 1;
}

/*
 * Provide the editor symbol navigation session select enclosing operation used by this
 * module and its client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_select_enclosing(
    UmiEditorSymbolNavigationSession *session,
    const UmiEditorSourceLocation *location)
{
    size_t index;
    size_t best = SIZE_MAX;
    size_t best_depth = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL ||
        umi_editor_source_location_validate(location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        const UmiEditorSourceLocation *symbol_location =
            &session->entries[index].symbol.location;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->entries[index].visible &&
            location_contains(symbol_location, location) &&
            (best == SIZE_MAX || session->entries[index].depth >= best_depth)) {
            best = index;
            best_depth = session->entries[index].depth;
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (best == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return umi_editor_symbol_navigation_session_select(session, best);
}

/*
 * Find editor symbol navigation session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_symbol_navigation_session_at(
    const UmiEditorSymbolNavigationSession *session,
    size_t position,
    UmiEditorSymbolNavigationEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= session->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = session->entries[position];
    return UMI_STATUS_OK;
}

/*
 * Find editor symbol navigation session visible while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_symbol_navigation_session_visible_at(
    const UmiEditorSymbolNavigationSession *session,
    size_t visible_position,
    UmiEditorSymbolNavigationEntry *out_entry)
{
    size_t index;
    size_t current = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < session->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (session->entries[index].visible) {
            /* Apply this operation only while the related capability or state is available. */
            if (current == visible_position) {
                *out_entry = session->entries[index];
                return UMI_STATUS_OK;
            }
            ++current;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find editor symbol navigation session while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_symbol_navigation_session_selected(
    const UmiEditorSymbolNavigationSession *session,
    UmiEditorSymbolNavigationEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (session->selected_index == SIZE_MAX ||
        session->selected_index >= session->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_entry = session->entries[session->selected_index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor symbol navigation session selected
 * path without changing their state.
 */
size_t umi_editor_symbol_navigation_session_selected_path_count(
    const UmiEditorSymbolNavigationSession *session)
{
    return session != NULL ? session->path_count : 0U;
}

/*
 * Find editor symbol navigation session selected path while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_editor_symbol_navigation_session_selected_path_at(
    const UmiEditorSymbolNavigationSession *session,
    size_t path_position,
    UmiEditorSymbolNavigationEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (path_position >= session->path_count) return UMI_STATUS_NOT_FOUND;
    *out_entry = session->entries[session->path_indices[path_position]];
    return UMI_STATUS_OK;
}

/*
 * Find editor symbol navigation session provider report while leaving the underlying
 * catalogue or model owned by this module.
 */
UmiStatus umi_editor_symbol_navigation_session_provider_report_at(
    const UmiEditorSymbolNavigationSession *session,
    size_t position,
    UmiEditorNavigationProviderReport *out_report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (position >= session->report_count) return UMI_STATUS_NOT_FOUND;
    *out_report = session->reports[position];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor symbol navigation session snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_symbol_navigation_session_snapshot(
    const UmiEditorSymbolNavigationSession *session,
    UmiEditorSymbolNavigationSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (session == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version =
        UMI_EDITOR_SYMBOL_NAVIGATION_SESSION_API_VERSION;
    out_snapshot->state = session->state;
    out_snapshot->scope = session->scope;
    out_snapshot->symbol_count = session->count;
    out_snapshot->visible_symbol_count = session->visible_count;
    out_snapshot->selected_index = session->selected_index;
    out_snapshot->selected_path_count = session->path_count;
    out_snapshot->provider_count = session->report_count;
    out_snapshot->request_id = session->has_request
        ? session->request.request_id
        : 0U;
    out_snapshot->revision = session->revision;
    out_snapshot->has_selection = session->selected_index != SIZE_MAX;
    out_snapshot->follow_cursor = session->follow_cursor;
    out_snapshot->truncated = session->truncated;
    (void)copy_text(out_snapshot->filter, sizeof(out_snapshot->filter),
                    session->filter);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by editor symbol navigation session without
 * changing their state.
 */
size_t umi_editor_symbol_navigation_session_count(
    const UmiEditorSymbolNavigationSession *session)
{
    return session != NULL ? session->count : 0U;
}

/*
 * Return the number of records represented by editor symbol navigation session visible
 * without changing their state.
 */
size_t umi_editor_symbol_navigation_session_visible_count(
    const UmiEditorSymbolNavigationSession *session)
{
    return session != NULL ? session->visible_count : 0U;
}

/*
 * Provide the editor symbol navigation session revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_symbol_navigation_session_revision(
    const UmiEditorSymbolNavigationSession *session)
{
    return session != NULL ? session->revision : 0U;
}
