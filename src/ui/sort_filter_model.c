/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/sort_filter_model.c
 *
 * PURPOSE:
 *   Implement reusable filter and sort descriptors shared by list, tree and table views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiUiSortFilterRegistry *registry, const char *id)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < registry->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[i].id, id) == 0) return i;
    }
    return SIZE_MAX;
}

/*
 * Initialise ui sort filter model registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ui_sort_filter_model_registry_create(UmiUiSortFilterRegistry **out_registry)
{
    UmiUiSortFilterRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiUiSortFilterRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ui sort filter model registry so the same storage can be
 * reused safely.
 */
void umi_ui_sort_filter_model_registry_destroy(UmiUiSortFilterRegistry *registry)
{
    free(registry);
}

/*
 * Provide the ui sort filter model registry upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_ui_sort_filter_model_registry_upsert(UmiUiSortFilterRegistry *registry, const UmiUiSortFilterSnapshot *item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || item == NULL || item->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, item->id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Remove ui sort filter model registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_ui_sort_filter_model_registry_remove(UmiUiSortFilterRegistry *registry, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < registry->count) {
        memmove(&registry->items[index], &registry->items[index + 1U],
                (registry->count - index - 1U) * sizeof(registry->items[0]));
    }
    registry->count -= 1U;
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find ui sort filter model registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_sort_filter_model_registry_find(const UmiUiSortFilterRegistry *registry, const char *id, UmiUiSortFilterSnapshot *out_item)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || id == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(registry, id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find ui sort filter model registry while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ui_sort_filter_model_registry_at(const UmiUiSortFilterRegistry *registry, size_t index, UmiUiSortFilterSnapshot *out_item)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;
    *out_item = registry->items[index];
    return UMI_STATUS_OK;
}

/* Provide the ascii lower operation used by this module and its client applications. */
static unsigned char ascii_lower(unsigned char value)
{
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (value >= (unsigned char)'A' && value <= (unsigned char)'Z') {
        return (unsigned char)(value - (unsigned char)'A' + (unsigned char)'a');
    }
    return value;
}

/* Find text equal while leaving the underlying catalogue or model owned by this module. */
static int text_equal_at(const char *text, const char *query, int case_sensitive)
{
    size_t index = 0U;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (query[index] != '\0') {
        unsigned char left;
        unsigned char right;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (text[index] == '\0') return 0;
        left = (unsigned char)text[index];
        right = (unsigned char)query[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!case_sensitive) {
            left = ascii_lower(left);
            right = ascii_lower(right);
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (left != right) return 0;
        index += 1U;
    }
    return 1;
}

/*
 * Provide the ui sort filter model matches operation used by this module and its client
 * applications.
 */
int umi_ui_sort_filter_model_matches(const UmiUiSortFilterSnapshot *filter,
                                     const char *text)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (filter == NULL || text == NULL) return 0;
    /* Apply this operation only while the related capability or state is available. */
    if (!filter->enabled || filter->query[0] == '\0') return 1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; text[index] != '\0'; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (text_equal_at(&text[index], filter->query, filter->case_sensitive)) {
            return 1;
        }
    }
    return 0;
}

/*
 * Provide the ui sort filter model compare text operation used by this module and its
 * client applications.
 */
int umi_ui_sort_filter_model_compare_text(const UmiUiSortFilterSnapshot *filter,
                                          const char *left,
                                          const char *right)
{
    size_t index = 0U;
    int result = 0;
    int case_sensitive;
    int ascending;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL && right == NULL) return 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (left == NULL) return -1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (right == NULL) return 1;

    case_sensitive = filter != NULL ? filter->case_sensitive : 1;
    ascending = filter == NULL || filter->ascending != 0;

    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (left[index] != '\0' || right[index] != '\0') {
        unsigned char left_value = (unsigned char)left[index];
        unsigned char right_value = (unsigned char)right[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!case_sensitive) {
            left_value = ascii_lower(left_value);
            right_value = ascii_lower(right_value);
        }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (left_value < right_value) { result = -1; break; }
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (left_value > right_value) { result = 1; break; }
        index += 1U;
    }
    return ascending ? result : -result;
}
/*
 * Return the number of records represented by ui sort filter model registry without
 * changing their state.
 */
size_t umi_ui_sort_filter_model_registry_count(const UmiUiSortFilterRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the ui sort filter model registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_ui_sort_filter_model_registry_revision(const UmiUiSortFilterRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
