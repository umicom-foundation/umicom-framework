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

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static int text_contains(const char *text, const char *needle)
{
    size_t text_index;
    size_t needle_length;
    if (needle == NULL || needle[0] == '\0') return 1;
    if (text == NULL) return 0;
    needle_length = strlen(needle);
    for (text_index = 0U; text[text_index] != '\0'; ++text_index) {
        size_t needle_index = 0U;
        while (needle_index < needle_length &&
               text[text_index + needle_index] != '\0' &&
               tolower((unsigned char)text[text_index + needle_index]) ==
                   tolower((unsigned char)needle[needle_index])) {
            ++needle_index;
        }
        if (needle_index == needle_length) return 1;
    }
    return 0;
}

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

static int compare_breakpoints(const UmiDebugBreakpointSnapshot *a,
                               const UmiDebugBreakpointSnapshot *b,
                               UmiDebugBreakpointSort sort)
{
    int comparison;
    if (sort == UMI_DEBUG_BREAKPOINT_SORT_VERIFICATION &&
        a->verified != b->verified) return b->verified - a->verified;
    comparison = strcmp(a->uri, b->uri);
    if (sort == UMI_DEBUG_BREAKPOINT_SORT_SOURCE_DESCENDING) {
        comparison = -comparison;
    }
    if (comparison != 0) return comparison;
    if (a->line < b->line) return -1;
    if (a->line > b->line) return 1;
    if (a->column < b->column) return -1;
    if (a->column > b->column) return 1;
    return strcmp(a->id, b->id);
}

static void sort_items(UmiDebugBreakpointQuery *query,
                       UmiDebugBreakpointSort sort)
{
    size_t index;
    for (index = 1U; index < query->count; ++index) {
        UmiDebugBreakpointSnapshot item = query->items[index];
        size_t insertion = index;
        while (insertion > 0U &&
               compare_breakpoints(&item, &query->items[insertion - 1U],
                                   sort) < 0) {
            query->items[insertion] = query->items[insertion - 1U];
            --insertion;
        }
        query->items[insertion] = item;
    }
}

static UmiStatus reserve_items(UmiDebugBreakpointQuery *query,
                               size_t required)
{
    size_t capacity;
    UmiDebugBreakpointSnapshot *replacement;
    if (required <= query->capacity) return UMI_STATUS_OK;
    capacity = query->capacity > 0U ? query->capacity : 16U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDebugBreakpointSnapshot *)realloc(
        query->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    query->items = replacement;
    query->capacity = capacity;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_breakpoint_query_create(
    UmiDebugBreakpointQuery **out_query)
{
    UmiDebugBreakpointQuery *query;
    if (out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_query = NULL;
    query = (UmiDebugBreakpointQuery *)calloc(1U, sizeof(*query));
    if (query == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    query->snapshot.revision = 1U;
    *out_query = query;
    return UMI_STATUS_OK;
}

void umi_debug_breakpoint_query_destroy(UmiDebugBreakpointQuery *query)
{
    if (query == NULL) return;
    free(query->items);
    free(query);
}

void umi_debug_breakpoint_filter_init(UmiDebugBreakpointFilter *filter)
{
    if (filter == NULL) return;
    (void)memset(filter, 0, sizeof(*filter));
    filter->struct_size = (uint32_t)sizeof(*filter);
    filter->api_version = UMI_DEBUG_BREAKPOINT_QUERY_API_VERSION;
    filter->sort = UMI_DEBUG_BREAKPOINT_SORT_SOURCE_ASCENDING;
    filter->enabled = -1;
    filter->verified = -1;
}

UmiStatus umi_debug_breakpoint_query_execute(
    UmiDebugBreakpointQuery *query,
    const UmiDebugBreakpointRegistry *registry,
    const UmiDebugBreakpointFilter *filter)
{
    size_t index;
    uint64_t revision;
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
    for (index = 0U; index < umi_debug_breakpoint_registry_count(registry);
         ++index) {
        UmiDebugBreakpointSnapshot item;
        UmiStatus status = umi_debug_breakpoint_registry_at(registry, index,
                                                            &item);
        if (status != UMI_STATUS_OK) return status;
        if (matches(&item, filter)) {
            status = reserve_items(query, query->count + 1U);
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
    for (index = 0U; index < query->count; ++index) {
        if (query->items[index].enabled) ++query->snapshot.enabled_count;
        if (query->items[index].verified) ++query->snapshot.verified_count;
        if (query->items[index].condition[0] != '\0') {
            ++query->snapshot.conditional_count;
        }
        if (query->items[index].log_message[0] != '\0') {
            ++query->snapshot.logpoint_count;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_breakpoint_query_at(
    const UmiDebugBreakpointQuery *query, size_t index,
    UmiDebugBreakpointSnapshot *out_breakpoint)
{
    if (query == NULL || out_breakpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= query->count) return UMI_STATUS_NOT_FOUND;
    *out_breakpoint = query->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_breakpoint_query_snapshot(
    const UmiDebugBreakpointQuery *query,
    UmiDebugBreakpointQuerySnapshot *out_snapshot)
{
    if (query == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = query->snapshot;
    return UMI_STATUS_OK;
}

size_t umi_debug_breakpoint_query_count(const UmiDebugBreakpointQuery *query)
{
    return query != NULL ? query->count : 0U;
}
