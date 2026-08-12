/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/sort_filter_model.c
 *
 * PURPOSE:
 *   Implement reusable filter and sort descriptors shared by list, tree and table views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Each operation is deliberately small and deterministic. Snapshots are copied
 * into bounded storage, revisions advance on mutation, and callers retain
 * responsibility for higher-level threading and persistence policy.
 */
#include "umicom/ui/sort_filter_model.h"

#include <stdlib.h>
#include <string.h>

struct UmiUiSortFilterRegistry {
    UmiUiSortFilterSnapshot items[UMI_UI_SORT_FILTER_MODEL_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiUiSortFilterRegistry *registry, const char *id)
{
    size_t i;
    if (registry == NULL || id == NULL) return SIZE_MAX;
    for (i = 0U; i < registry->count; ++i) {
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

UmiStatus umi_ui_sort_filter_model_registry_create(UmiUiSortFilterRegistry **out_registry)
{
    UmiUiSortFilterRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiUiSortFilterRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_ui_sort_filter_model_registry_destroy(UmiUiSortFilterRegistry *registry)
{
    free(registry);
}

UmiStatus umi_ui_sort_filter_model_registry_upsert(UmiUiSortFilterRegistry *registry, const UmiUiSortFilterSnapshot *item)
{
    size_t index;
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    if (index == SIZE_MAX) {
        if (registry->count >= UMI_UI_SORT_FILTER_MODEL_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
        index = registry->count++;
    }
    registry->items[index] = *item;
    registry->items[index].struct_size = (uint32_t)sizeof(UmiUiSortFilterSnapshot);
    registry->items[index].api_version = 1U;
    registry->revision += 1U;
    registry->items[index].revision = registry->revision;
    
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_sort_filter_model_registry_remove(UmiUiSortFilterRegistry *registry, const char *id)
{
    size_t index;
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count - index - 1U) * sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_sort_filter_model_registry_find(const UmiUiSortFilterRegistry *registry, const char *id, UmiUiSortFilterSnapshot *out_item)
{
    size_t index;
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_ui_sort_filter_model_registry_at(const UmiUiSortFilterRegistry *registry, size_t index, UmiUiSortFilterSnapshot *out_item)
{
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

static unsigned char ascii_lower(unsigned char value)
{
    if (value >= (unsigned char)'A' && value <= (unsigned char)'Z') {
        return (unsigned char)(value - (unsigned char)'A' + (unsigned char)'a');
    }
    return value;
}

static int text_equal_at(const char *text, const char *query, int case_sensitive)
{
    size_t index = 0U;
    while (query[index] != '\0') {
        unsigned char left;
        unsigned char right;
        if (text[index] == '\0') return 0;
        left = (unsigned char)text[index];
        right = (unsigned char)query[index];
        if (!case_sensitive) {
            left = ascii_lower(left);
            right = ascii_lower(right);
        }
        if (left != right) return 0;
        index += 1U;
    }
    return 1;
}

int umi_ui_sort_filter_model_matches(const UmiUiSortFilterSnapshot *filter,
                                     const char *text)
{
    size_t index;
    if (filter == NULL || text == NULL) return 0;
    if (!filter->enabled || filter->query[0] == '\0') return 1;
    for (index = 0U; text[index] != '\0'; ++index) {
        if (text_equal_at(&text[index], filter->query, filter->case_sensitive)) {
            return 1;
        }
    }
    return 0;
}

int umi_ui_sort_filter_model_compare_text(const UmiUiSortFilterSnapshot *filter,
                                          const char *left,
                                          const char *right)
{
    size_t index = 0U;
    int result = 0;
    int case_sensitive;
    int ascending;

    if (left == NULL && right == NULL) return 0;
    if (left == NULL) return -1;
    if (right == NULL) return 1;

    case_sensitive = filter != NULL ? filter->case_sensitive : 1;
    ascending = filter == NULL || filter->ascending != 0;

    while (left[index] != '\0' || right[index] != '\0') {
        unsigned char left_value = (unsigned char)left[index];
        unsigned char right_value = (unsigned char)right[index];
        if (!case_sensitive) {
            left_value = ascii_lower(left_value);
            right_value = ascii_lower(right_value);
        }
        if (left_value < right_value) { result = -1; break; }
        if (left_value > right_value) { result = 1; break; }
        index += 1U;
    }
    return ascending ? result : -result;
}
size_t umi_ui_sort_filter_model_registry_count(const UmiUiSortFilterRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_ui_sort_filter_model_registry_revision(const UmiUiSortFilterRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
