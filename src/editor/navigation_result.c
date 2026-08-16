/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/navigation_result.c
 *
 * PURPOSE:
 *   Implement an owned, dynamically sized source result set. Results from
 *   several providers coalesce by source position and are sorted by primary
 *   status, rank and deterministic source order.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/navigation_result.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorNavigationResultSet {
    UmiEditorNavigationResult *items;
    size_t count;
    size_t capacity;
    size_t selected_index;
    UmiEditorNavigationQueryKind query_kind;
    uint64_t next_sequence;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

static int valid_query_kind(UmiEditorNavigationQueryKind kind)
{
    return kind >= UMI_EDITOR_NAVIGATION_QUERY_DEFINITION &&
           kind <= UMI_EDITOR_NAVIGATION_QUERY_WORKSPACE_SYMBOL;
}

static int text_terminated(const char *text, size_t capacity)
{
    return text != NULL && memchr(text, '\0', capacity) != NULL;
}

static UmiStatus validate_result(const UmiEditorNavigationResult *result)
{
    if (result == NULL ||
        result->struct_size != (uint32_t)sizeof(*result) ||
        result->api_version != UMI_EDITOR_NAVIGATION_RESULT_API_VERSION ||
        !text_terminated(result->provider_id, sizeof(result->provider_id)) ||
        result->provider_id[0] == '\0' ||
        !text_terminated(result->detail, sizeof(result->detail))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_editor_source_location_validate(&result->location);
}

static UmiStatus reserve_results(UmiEditorNavigationResultSet *result_set,
                                 size_t required)
{
    size_t capacity;
    UmiEditorNavigationResult *replacement;

    if (required <= result_set->capacity) return UMI_STATUS_OK;
    capacity = result_set->capacity > 0U ? result_set->capacity : 32U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorNavigationResult *)realloc(
        result_set->items, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    result_set->items = replacement;
    result_set->capacity = capacity;
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_result_set_create(
    UmiEditorNavigationQueryKind query_kind,
    UmiEditorNavigationResultSet **out_result_set)
{
    UmiEditorNavigationResultSet *result_set;

    if (!valid_query_kind(query_kind) || out_result_set == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_result_set = NULL;
    result_set = (UmiEditorNavigationResultSet *)calloc(1U,
                                                        sizeof(*result_set));
    if (result_set == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    result_set->query_kind = query_kind;
    result_set->selected_index = SIZE_MAX;
    result_set->next_sequence = 1U;
    result_set->revision = 1U;
    *out_result_set = result_set;
    return UMI_STATUS_OK;
}

void umi_editor_navigation_result_set_destroy(
    UmiEditorNavigationResultSet *result_set)
{
    if (result_set == NULL) return;
    free(result_set->items);
    result_set->items = NULL;
    free(result_set);
}

UmiStatus umi_editor_navigation_result_set_reset(
    UmiEditorNavigationResultSet *result_set,
    UmiEditorNavigationQueryKind query_kind)
{
    if (result_set == NULL || !valid_query_kind(query_kind)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    result_set->count = 0U;
    result_set->selected_index = SIZE_MAX;
    result_set->query_kind = query_kind;
    result_set->next_sequence = 1U;
    result_set->revision = next_revision(result_set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_result_set_upsert(
    UmiEditorNavigationResultSet *result_set,
    const UmiEditorNavigationResult *result)
{
    size_t index;
    UmiEditorNavigationResult stored;
    UmiStatus status;

    if (result_set == NULL || validate_result(result) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < result_set->count; ++index) {
        if (strcmp(result_set->items[index].provider_id,
                   result->provider_id) == 0 &&
            umi_editor_source_location_same_position(
                &result_set->items[index].location, &result->location)) {
            stored = *result;
            stored.sequence = result_set->items[index].sequence;
            result_set->items[index] = stored;
            result_set->revision = next_revision(result_set->revision);
            return UMI_STATUS_OK;
        }
    }
    status = reserve_results(result_set, result_set->count + 1U);
    if (status != UMI_STATUS_OK) return status;
    stored = *result;
    stored.sequence = result_set->next_sequence++;
    if (result_set->next_sequence == 0U) result_set->next_sequence = 1U;
    result_set->items[result_set->count++] = stored;
    if (result_set->selected_index == SIZE_MAX) {
        result_set->selected_index = 0U;
    }
    result_set->revision = next_revision(result_set->revision);
    return UMI_STATUS_OK;
}

static int compare_results(const void *left_value, const void *right_value)
{
    const UmiEditorNavigationResult *left =
        (const UmiEditorNavigationResult *)left_value;
    const UmiEditorNavigationResult *right =
        (const UmiEditorNavigationResult *)right_value;
    int location_order;

    if (left->primary != right->primary) return right->primary - left->primary;
    if (left->rank < right->rank) return 1;
    if (left->rank > right->rank) return -1;
    location_order = umi_editor_source_location_compare(&left->location,
                                                         &right->location);
    if (location_order != 0) return location_order;
    if (left->sequence < right->sequence) return -1;
    if (left->sequence > right->sequence) return 1;
    return 0;
}

UmiStatus umi_editor_navigation_result_set_sort(
    UmiEditorNavigationResultSet *result_set)
{
    uint64_t selected_sequence = 0U;
    size_t index;

    if (result_set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (result_set->selected_index < result_set->count) {
        selected_sequence = result_set->items[result_set->selected_index].sequence;
    }
    if (result_set->count > 1U) {
        qsort(result_set->items,
              result_set->count,
              sizeof(*result_set->items),
              compare_results);
    }
    result_set->selected_index = result_set->count > 0U ? 0U : SIZE_MAX;
    for (index = 0U; index < result_set->count; ++index) {
        if (result_set->items[index].sequence == selected_sequence) {
            result_set->selected_index = index;
            break;
        }
    }
    result_set->revision = next_revision(result_set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_result_set_select(
    UmiEditorNavigationResultSet *result_set,
    size_t index)
{
    if (result_set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= result_set->count) return UMI_STATUS_NOT_FOUND;
    result_set->selected_index = index;
    result_set->revision = next_revision(result_set->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_result_set_selected(
    const UmiEditorNavigationResultSet *result_set,
    UmiEditorNavigationResult *out_result)
{
    if (result_set == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (result_set->selected_index >= result_set->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_result = result_set->items[result_set->selected_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_result_set_at(
    const UmiEditorNavigationResultSet *result_set,
    size_t index,
    UmiEditorNavigationResult *out_result)
{
    if (result_set == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= result_set->count) return UMI_STATUS_NOT_FOUND;
    *out_result = result_set->items[index];
    return UMI_STATUS_OK;
}

size_t umi_editor_navigation_result_set_count(
    const UmiEditorNavigationResultSet *result_set)
{
    return result_set != NULL ? result_set->count : 0U;
}

size_t umi_editor_navigation_result_set_selected_index(
    const UmiEditorNavigationResultSet *result_set)
{
    return result_set != NULL ? result_set->selected_index : SIZE_MAX;
}

UmiEditorNavigationQueryKind umi_editor_navigation_result_set_query_kind(
    const UmiEditorNavigationResultSet *result_set)
{
    return result_set != NULL
        ? result_set->query_kind
        : (UmiEditorNavigationQueryKind)0;
}

uint64_t umi_editor_navigation_result_set_revision(
    const UmiEditorNavigationResultSet *result_set)
{
    return result_set != NULL ? result_set->revision : 0U;
}
