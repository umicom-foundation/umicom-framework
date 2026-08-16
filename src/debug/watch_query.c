/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug/watch_query.c
 *
 * PURPOSE:
 *   Implement reusable watch-expression filtering and ordering.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug/watch_query.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugWatchQuery {
    UmiDebugWatchSnapshot *items;
    size_t count;
    size_t capacity;
    UmiDebugWatchQuerySnapshot snapshot;
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
    size_t i;
    size_t length;
    if (needle == NULL || needle[0] == '\0') return 1;
    if (text == NULL) return 0;
    length = strlen(needle);
    for (i = 0U; text[i] != '\0'; ++i) {
        size_t j = 0U;
        while (j < length && text[i + j] != '\0' &&
               tolower((unsigned char)text[i + j]) ==
                   tolower((unsigned char)needle[j])) ++j;
        if (j == length) return 1;
    }
    return 0;
}

static int compare_watches(const UmiDebugWatchSnapshot *a,
                           const UmiDebugWatchSnapshot *b,
                           UmiDebugWatchSort sort)
{
    if (sort == UMI_DEBUG_WATCH_SORT_INVALID_FIRST &&
        a->valid != b->valid) return a->valid - b->valid;
    if (sort == UMI_DEBUG_WATCH_SORT_REGISTRY_ORDER) return 0;
    return strcmp(a->expression, b->expression);
}

static void sort_items(UmiDebugWatchQuery *query, UmiDebugWatchSort sort)
{
    size_t index;
    for (index = 1U; index < query->count; ++index) {
        UmiDebugWatchSnapshot item = query->items[index];
        size_t insertion = index;
        while (insertion > 0U &&
               compare_watches(&item, &query->items[insertion - 1U], sort) <
                   0) {
            query->items[insertion] = query->items[insertion - 1U];
            --insertion;
        }
        query->items[insertion] = item;
    }
}

static UmiStatus reserve_items(UmiDebugWatchQuery *query, size_t required)
{
    size_t capacity;
    UmiDebugWatchSnapshot *replacement;
    if (required <= query->capacity) return UMI_STATUS_OK;
    capacity = query->capacity > 0U ? query->capacity : 16U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiDebugWatchSnapshot *)realloc(
        query->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    query->items = replacement;
    query->capacity = capacity;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_watch_query_create(UmiDebugWatchQuery **out_query)
{
    UmiDebugWatchQuery *query;
    if (out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_query = NULL;
    query = (UmiDebugWatchQuery *)calloc(1U, sizeof(*query));
    if (query == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    query->snapshot.revision = 1U;
    *out_query = query;
    return UMI_STATUS_OK;
}

void umi_debug_watch_query_destroy(UmiDebugWatchQuery *query)
{
    if (query == NULL) return;
    free(query->items);
    free(query);
}

void umi_debug_watch_filter_init(UmiDebugWatchFilter *filter)
{
    if (filter == NULL) return;
    (void)memset(filter, 0, sizeof(*filter));
    filter->struct_size = (uint32_t)sizeof(*filter);
    filter->api_version = UMI_DEBUG_WATCH_QUERY_API_VERSION;
    filter->sort = UMI_DEBUG_WATCH_SORT_REGISTRY_ORDER;
    filter->enabled = -1;
    filter->valid = -1;
}

UmiStatus umi_debug_watch_query_execute(
    UmiDebugWatchQuery *query, const UmiDebugWatchRegistry *registry,
    const UmiDebugWatchFilter *filter)
{
    size_t index;
    uint64_t revision;
    if (query == NULL || registry == NULL || filter == NULL ||
        filter->struct_size != (uint32_t)sizeof(*filter) ||
        filter->api_version != UMI_DEBUG_WATCH_QUERY_API_VERSION ||
        !terminated(filter->session_id, sizeof(filter->session_id)) ||
        !terminated(filter->text, sizeof(filter->text)) ||
        filter->sort < UMI_DEBUG_WATCH_SORT_REGISTRY_ORDER ||
        filter->sort > UMI_DEBUG_WATCH_SORT_INVALID_FIRST ||
        filter->enabled < -1 || filter->enabled > 1 ||
        filter->valid < -1 || filter->valid > 1) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    revision = next_revision(query->snapshot.revision);
    query->count = 0U;
    (void)memset(&query->snapshot, 0, sizeof(query->snapshot));
    for (index = 0U; index < umi_debug_watch_registry_count(registry); ++index) {
        UmiDebugWatchSnapshot item;
        UmiStatus status = umi_debug_watch_registry_at(registry, index, &item);
        int matched;
        if (status != UMI_STATUS_OK) return status;
        matched = (filter->session_id[0] == '\0' ||
                   strcmp(item.session_id, filter->session_id) == 0) &&
                  (filter->enabled < 0 || item.enabled == filter->enabled) &&
                  (filter->valid < 0 || item.valid == filter->valid) &&
                  (text_contains(item.expression, filter->text) ||
                   text_contains(item.value, filter->text) ||
                   text_contains(item.type, filter->text));
        if (matched) {
            status = reserve_items(query, query->count + 1U);
            if (status != UMI_STATUS_OK) return status;
            query->items[query->count++] = item;
        }
    }
    if (filter->sort != UMI_DEBUG_WATCH_SORT_REGISTRY_ORDER) {
        sort_items(query, filter->sort);
    }
    query->snapshot.struct_size = (uint32_t)sizeof(query->snapshot);
    query->snapshot.api_version = UMI_DEBUG_WATCH_QUERY_API_VERSION;
    query->snapshot.result_count = query->count;
    query->snapshot.source_revision = umi_debug_watch_registry_revision(registry);
    query->snapshot.revision = revision;
    for (index = 0U; index < query->count; ++index) {
        if (query->items[index].enabled) ++query->snapshot.enabled_count;
        if (query->items[index].valid) ++query->snapshot.valid_count;
        else ++query->snapshot.invalid_count;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_watch_query_at(const UmiDebugWatchQuery *query,
                                   size_t index,
                                   UmiDebugWatchSnapshot *out_watch)
{
    if (query == NULL || out_watch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= query->count) return UMI_STATUS_NOT_FOUND;
    *out_watch = query->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_watch_query_snapshot(
    const UmiDebugWatchQuery *query,
    UmiDebugWatchQuerySnapshot *out_snapshot)
{
    if (query == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_snapshot = query->snapshot;
    return UMI_STATUS_OK;
}

size_t umi_debug_watch_query_count(const UmiDebugWatchQuery *query)
{
    return query != NULL ? query->count : 0U;
}
