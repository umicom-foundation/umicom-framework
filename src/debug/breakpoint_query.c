/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/breakpoint_query.c
 *
 * PURPOSE:
 *   Implement deterministic breakpoint filtering, ordering and summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/breakpoint_query.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugBreakpointQuery {
    UmiDebugBreakpointSnapshot *items;
    size_t count;
    size_t capacity;
    UmiDebugBreakpointQuerySnapshot snapshot;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

/* Provide the terminated operation used by this module and its client applications. */
static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

/* Provide the text contains operation used by this module and its client applications. */
static int text_contains(const char *text, const char *needle)
{
    size_t text_index;
    size_t needle_length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (needle == NULL || needle[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    needle_length = strlen(needle);
    /* Visit each bounded item once so every record receives the same rule. */
    for (text_index = 0U; text[text_index] != '\0'; ++text_index) {
        size_t needle_index = 0U;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (needle_index < needle_length &&
               text[text_index + needle_index] != '\0' &&
               tolower((unsigned char)text[text_index + needle_index]) ==
                   tolower((unsigned char)needle[needle_index])) {
            ++needle_index;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (needle_index == needle_length) return 1;
    }
    return 0;
}

/* Provide the matches operation used by this module and its client applications. */
static int matches(const UmiDebugBreakpointSnapshot *item,
                   const UmiDebugBreakpointFilter *filter)
{
    return (filter->session_id[0] == '\0' ||
            strcmp(item->session_id, filter->session_id) == 0) &&
           (filter->uri[0] == '\0' || strcmp(item->uri, filter->uri) == 0) &&
           (filter->enabled < 0 || item->enabled == filter->enabled) &&
           (filter->verified < 0 || item->verified == filter->verified) &&
           (text_contains(item->uri, filter->text) ||
            text_contains(item->condition, filter->text) ||
            text_contains(item->log_message, filter->text));
}

/*
 * Provide the compare breakpoints operation used by this module and its client
 * applications.
 */
static int compare_breakpoints(const UmiDebugBreakpointSnapshot *a,
                               const UmiDebugBreakpointSnapshot *b,
                               UmiDebugBreakpointSort sort)
{
    int comparison;
    /* Apply this branch only when its contract condition is satisfied. */
    if (sort == UMI_DEBUG_BREAKPOINT_SORT_VERIFICATION &&
        a->verified != b->verified) return b->verified - a->verified;
    comparison = strcmp(a->uri, b->uri);
    /* Apply this branch only when its contract condition is satisfied. */
    if (sort == UMI_DEBUG_BREAKPOINT_SORT_SOURCE_DESCENDING) {
        comparison = -comparison;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (comparison != 0) return comparison;
    /* Apply this branch only when its contract condition is satisfied. */
    if (a->line < b->line) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (a->line > b->line) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (a->column < b->column) return -1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (a->column > b->column) return 1;
    return strcmp(a->id, b->id);
}

/* Provide the sort items operation used by this module and its client applications. */
static void sort_items(UmiDebugBreakpointQuery *query,
                       UmiDebugBreakpointSort sort)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 1U; index < query->count; ++index) {
        UmiDebugBreakpointSnapshot item = query->items[index];
        size_t insertion = index;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (insertion > 0U &&
               compare_breakpoints(&item, &query->items[insertion - 1U],
                                   sort) < 0) {
            query->items[insertion] = query->items[insertion - 1U];
            --insertion;
        }
        query->items[insertion] = item;
    }
}

/* Provide the reserve items operation used by this module and its client applications. */
static UmiStatus reserve_items(UmiDebugBreakpointQuery *query,
                               size_t required)
{
    size_t capacity;
    UmiDebugBreakpointSnapshot *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= query->capacity) return UMI_STATUS_OK;
    capacity = query->capacity > 0U ? query->capacity : 16U;
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
    replacement = (UmiDebugBreakpointSnapshot *)realloc(
        query->items, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    query->items = replacement;
    query->capacity = capacity;
    return UMI_STATUS_OK;
}

/*
 * Initialise debug breakpoint query from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_debug_breakpoint_query_create(
    UmiDebugBreakpointQuery **out_query)
{
    UmiDebugBreakpointQuery *query;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_query = NULL;
    query = (UmiDebugBreakpointQuery *)calloc(1U, sizeof(*query));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    query->snapshot.revision = 1U;
    *out_query = query;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by debug breakpoint query so the same storage can be reused
 * safely.
 */
void umi_debug_breakpoint_query_destroy(UmiDebugBreakpointQuery *query)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL) return;
    free(query->items);
    free(query);
}

/*
 * Initialise debug breakpoint filter from caller-provided values so later operations
 * receive a known state.
 */
void umi_debug_breakpoint_filter_init(UmiDebugBreakpointFilter *filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL) return;
    (void)memset(filter, 0, sizeof(*filter));
    filter->struct_size = (uint32_t)sizeof(*filter);
    filter->api_version = UMI_DEBUG_BREAKPOINT_QUERY_API_VERSION;
    filter->sort = UMI_DEBUG_BREAKPOINT_SORT_SOURCE_ASCENDING;
    filter->enabled = -1;
    filter->verified = -1;
}

/*
 * Perform debug breakpoint query through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_debug_breakpoint_query_execute(
    UmiDebugBreakpointQuery *query,
    const UmiDebugBreakpointRegistry *registry,
    const UmiDebugBreakpointFilter *filter)
{
    size_t index;
    uint64_t revision;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || registry == NULL || filter == NULL ||
        filter->struct_size != (uint32_t)sizeof(*filter) ||
        filter->api_version != UMI_DEBUG_BREAKPOINT_QUERY_API_VERSION ||
        !terminated(filter->session_id, sizeof(filter->session_id)) ||
        !terminated(filter->uri, sizeof(filter->uri)) ||
        !terminated(filter->text, sizeof(filter->text)) ||
        filter->sort < UMI_DEBUG_BREAKPOINT_SORT_SOURCE_ASCENDING ||
        filter->sort > UMI_DEBUG_BREAKPOINT_SORT_VERIFICATION ||
        filter->enabled < -1 || filter->enabled > 1 ||
        filter->verified < -1 || filter->verified > 1) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    revision = next_revision(query->snapshot.revision);
    query->count = 0U;
    (void)memset(&query->snapshot, 0, sizeof(query->snapshot));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_debug_breakpoint_registry_count(registry);
         ++index) {
        UmiDebugBreakpointSnapshot item;
        UmiStatus status = umi_debug_breakpoint_registry_at(registry, index,
                                                            &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (matches(&item, filter)) {
            status = reserve_items(query, query->count + 1U);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            query->items[query->count++] = item;
        }
    }
    sort_items(query, filter->sort);
    query->snapshot.struct_size = (uint32_t)sizeof(query->snapshot);
    query->snapshot.api_version = UMI_DEBUG_BREAKPOINT_QUERY_API_VERSION;
    query->snapshot.result_count = query->count;
    query->snapshot.source_revision =
        umi_debug_breakpoint_registry_revision(registry);
    query->snapshot.revision = revision;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < query->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (query->items[index].enabled) ++query->snapshot.enabled_count;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (query->items[index].verified) ++query->snapshot.verified_count;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (query->items[index].condition[0] != '\0') {
            ++query->snapshot.conditional_count;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (query->items[index].log_message[0] != '\0') {
            ++query->snapshot.logpoint_count;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Find debug breakpoint query while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_debug_breakpoint_query_at(
    const UmiDebugBreakpointQuery *query, size_t index,
    UmiDebugBreakpointSnapshot *out_breakpoint)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || out_breakpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= query->count) return UMI_STATUS_NOT_FOUND;
    *out_breakpoint = query->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the debug breakpoint query snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_breakpoint_query_snapshot(
    const UmiDebugBreakpointQuery *query,
    UmiDebugBreakpointQuerySnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = query->snapshot;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by debug breakpoint query without changing
 * their state.
 */
size_t umi_debug_breakpoint_query_count(const UmiDebugBreakpointQuery *query)
{
    return query != NULL ? query->count : 0U;
}
