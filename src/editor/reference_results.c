/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/reference_results.c
 *
 * PURPOSE:
 *   Implement grouped, filtered and keyboard-navigable reference results for
 *   native parsers, compiler indexes, language servers and AI providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/reference_results.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct UmiEditorReferenceResults {
    UmiEditorReferenceGroup *groups;
    size_t group_count;
    size_t group_capacity;
    UmiEditorReferenceEntry *entries;
    size_t count;
    size_t entry_capacity;
    size_t selected_index;
    char filter[UMI_EDITOR_REFERENCE_FILTER_CAPACITY];
    uint64_t revision;
};

static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

static UmiStatus copy_text(char *destination,
                           size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || source == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static UmiStatus reserve_groups(UmiEditorReferenceResults *results,
                                size_t required)
{
    size_t capacity;
    UmiEditorReferenceGroup *replacement;
    if (required <= results->group_capacity) return UMI_STATUS_OK;
    capacity = results->group_capacity > 0U ? results->group_capacity : 16U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorReferenceGroup *)realloc(
        results->groups, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    results->groups = replacement;
    results->group_capacity = capacity;
    return UMI_STATUS_OK;
}

static UmiStatus reserve_entries(UmiEditorReferenceResults *results,
                                 size_t required)
{
    size_t capacity;
    UmiEditorReferenceEntry *replacement;
    if (required <= results->entry_capacity) return UMI_STATUS_OK;
    capacity = results->entry_capacity > 0U ? results->entry_capacity : 64U;
    while (capacity < required) {
        if (capacity > SIZE_MAX / 2U) return UMI_STATUS_CAPACITY_EXCEEDED;
        capacity *= 2U;
    }
    if (capacity > SIZE_MAX / sizeof(*replacement)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    replacement = (UmiEditorReferenceEntry *)realloc(
        results->entries, capacity * sizeof(*replacement));
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    results->entries = replacement;
    results->entry_capacity = capacity;
    return UMI_STATUS_OK;
}

static size_t find_group(const UmiEditorReferenceResults *results,
                         const char *uri)
{
    size_t index;
    for (index = 0U; index < results->group_count; ++index) {
        if (strcmp(results->groups[index].uri, uri) == 0) return index;
    }
    return SIZE_MAX;
}

static const char *file_label(const char *uri)
{
    const char *cursor;
    const char *label = uri;
    for (cursor = uri; *cursor != '\0'; ++cursor) {
        if (*cursor == '/' || *cursor == '\\') label = cursor + 1;
    }
    return label[0] != '\0' ? label : uri;
}

static int contains_case_insensitive(const char *text, const char *needle)
{
    const char *start;
    if (needle[0] == '\0') return 1;
    for (start = text; *start != '\0'; ++start) {
        const char *left = start;
        const char *right = needle;
        while (*left != '\0' && *right != '\0' &&
               tolower((unsigned char)*left) ==
                   tolower((unsigned char)*right)) {
            left += 1;
            right += 1;
        }
        if (*right == '\0') return 1;
    }
    return 0;
}

static int entry_matches(const UmiEditorReferenceEntry *entry,
                         const char *filter)
{
    return contains_case_insensitive(entry->result.location.uri, filter) ||
           contains_case_insensitive(entry->result.location.label, filter) ||
           contains_case_insensitive(entry->result.location.preview, filter) ||
           contains_case_insensitive(entry->result.detail, filter);
}

static void refresh_visibility(UmiEditorReferenceResults *results)
{
    size_t group_index;
    size_t entry_index;
    size_t first_visible = SIZE_MAX;

    for (group_index = 0U; group_index < results->group_count; ++group_index) {
        results->groups[group_index].visible_result_count = 0U;
    }
    for (entry_index = 0U; entry_index < results->count; ++entry_index) {
        UmiEditorReferenceEntry *entry = &results->entries[entry_index];
        UmiEditorReferenceGroup *group = &results->groups[entry->group_index];
        entry->visible = group->expanded &&
            entry_matches(entry, results->filter);
        if (entry->visible) {
            group->visible_result_count += 1U;
            if (first_visible == SIZE_MAX) first_visible = entry_index;
        }
        entry->selected = 0;
    }
    if (results->selected_index >= results->count ||
        !results->entries[results->selected_index].visible) {
        results->selected_index = first_visible;
    }
    if (results->selected_index < results->count) {
        results->entries[results->selected_index].selected = 1;
    }
}

UmiStatus umi_editor_reference_results_create(
    UmiEditorReferenceResults **out_results)
{
    UmiEditorReferenceResults *results;
    if (out_results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_results = NULL;
    results = (UmiEditorReferenceResults *)calloc(1U, sizeof(*results));
    if (results == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    results->selected_index = SIZE_MAX;
    results->revision = 1U;
    *out_results = results;
    return UMI_STATUS_OK;
}

void umi_editor_reference_results_destroy(UmiEditorReferenceResults *results)
{
    if (results == NULL) return;
    free(results->entries);
    free(results->groups);
    results->entries = NULL;
    results->groups = NULL;
    free(results);
}

UmiStatus umi_editor_reference_results_load(
    UmiEditorReferenceResults *results,
    const UmiEditorNavigationResultSet *navigation_results)
{
    size_t count;
    size_t index;
    UmiStatus status;

    if (results == NULL || navigation_results == NULL ||
        umi_editor_navigation_result_set_query_kind(navigation_results) !=
            UMI_EDITOR_NAVIGATION_QUERY_REFERENCE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = umi_editor_navigation_result_set_count(navigation_results);
    status = reserve_entries(results, count);
    if (status != UMI_STATUS_OK) return status;
    results->count = 0U;
    results->group_count = 0U;
    results->selected_index = SIZE_MAX;
    results->filter[0] = '\0';
    for (index = 0U; index < count; ++index) {
        UmiEditorNavigationResult navigation_result;
        UmiEditorReferenceEntry entry = {0};
        size_t group_index;

        status = umi_editor_navigation_result_set_at(navigation_results,
                                                      index,
                                                      &navigation_result);
        if (status != UMI_STATUS_OK) return status;
        group_index = find_group(results, navigation_result.location.uri);
        if (group_index == SIZE_MAX) {
            UmiEditorReferenceGroup group = {0};
            status = reserve_groups(results, results->group_count + 1U);
            if (status != UMI_STATUS_OK) return status;
            group.struct_size = (uint32_t)sizeof(group);
            group.api_version = UMI_EDITOR_REFERENCE_RESULTS_API_VERSION;
            group.expanded = 1;
            status = copy_text(group.uri, sizeof(group.uri),
                               navigation_result.location.uri);
            if (status == UMI_STATUS_OK) {
                status = copy_text(group.label,
                                   sizeof(group.label),
                                   file_label(navigation_result.location.uri));
            }
            if (status != UMI_STATUS_OK) return status;
            group_index = results->group_count;
            results->groups[results->group_count++] = group;
        }
        entry.struct_size = (uint32_t)sizeof(entry);
        entry.api_version = UMI_EDITOR_REFERENCE_RESULTS_API_VERSION;
        entry.result = navigation_result;
        entry.group_index = group_index;
        results->entries[results->count++] = entry;
        results->groups[group_index].result_count += 1U;
    }
    refresh_visibility(results);
    results->revision = next_revision(results->revision);
    return count > 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_reference_results_set_filter(
    UmiEditorReferenceResults *results,
    const char *filter)
{
    UmiStatus status;
    if (results == NULL || filter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(results->filter, sizeof(results->filter), filter);
    if (status != UMI_STATUS_OK) return status;
    refresh_visibility(results);
    results->revision = next_revision(results->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_results_set_group_expanded(
    UmiEditorReferenceResults *results,
    size_t group_index,
    int expanded)
{
    if (results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (group_index >= results->group_count) return UMI_STATUS_NOT_FOUND;
    results->groups[group_index].expanded = expanded != 0;
    refresh_visibility(results);
    results->revision = next_revision(results->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_results_select(
    UmiEditorReferenceResults *results,
    size_t result_index)
{
    if (results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (result_index >= results->count ||
        !results->entries[result_index].visible) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (results->selected_index < results->count) {
        results->entries[results->selected_index].selected = 0;
    }
    results->selected_index = result_index;
    results->entries[result_index].selected = 1;
    results->revision = next_revision(results->revision);
    return UMI_STATUS_OK;
}

static UmiStatus select_relative(UmiEditorReferenceResults *results,
                                 int forward,
                                 int wrap)
{
    size_t index;
    size_t visited;
    if (results->count == 0U) return UMI_STATUS_NOT_FOUND;
    index = results->selected_index < results->count
        ? results->selected_index
        : (forward ? results->count - 1U : 0U);
    for (visited = 0U; visited < results->count; ++visited) {
        if (forward) {
            if (index + 1U < results->count) index += 1U;
            else if (wrap) index = 0U;
            else return UMI_STATUS_NOT_FOUND;
        } else {
            if (index > 0U) index -= 1U;
            else if (wrap) index = results->count - 1U;
            else return UMI_STATUS_NOT_FOUND;
        }
        if (results->entries[index].visible) {
            return umi_editor_reference_results_select(results, index);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_reference_results_select_next(
    UmiEditorReferenceResults *results,
    int wrap)
{
    if (results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return select_relative(results, 1, wrap);
}

UmiStatus umi_editor_reference_results_select_previous(
    UmiEditorReferenceResults *results,
    int wrap)
{
    if (results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return select_relative(results, 0, wrap);
}

UmiStatus umi_editor_reference_results_group_at(
    const UmiEditorReferenceResults *results,
    size_t group_index,
    UmiEditorReferenceGroup *out_group)
{
    if (results == NULL || out_group == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (group_index >= results->group_count) return UMI_STATUS_NOT_FOUND;
    *out_group = results->groups[group_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_results_entry_at(
    const UmiEditorReferenceResults *results,
    size_t result_index,
    UmiEditorReferenceEntry *out_entry)
{
    if (results == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (result_index >= results->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = results->entries[result_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_results_visible_entry_at(
    const UmiEditorReferenceResults *results,
    size_t visible_index,
    UmiEditorReferenceEntry *out_entry)
{
    size_t index;
    size_t current = 0U;
    if (results == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < results->count; ++index) {
        if (!results->entries[index].visible) continue;
        if (current == visible_index) {
            *out_entry = results->entries[index];
            return UMI_STATUS_OK;
        }
        current += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_editor_reference_results_selected(
    const UmiEditorReferenceResults *results,
    UmiEditorReferenceEntry *out_entry)
{
    if (results == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (results->selected_index >= results->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = results->entries[results->selected_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_reference_results_snapshot(
    const UmiEditorReferenceResults *results,
    UmiEditorReferenceResultsSnapshot *out_snapshot)
{
    if (results == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_REFERENCE_RESULTS_API_VERSION;
    out_snapshot->group_count = results->group_count;
    out_snapshot->result_count = results->count;
    out_snapshot->visible_result_count =
        umi_editor_reference_results_visible_count(results);
    out_snapshot->selected_index = results->selected_index < results->count
        ? results->selected_index
        : 0U;
    (void)memcpy(out_snapshot->filter,
                 results->filter,
                 strlen(results->filter) + 1U);
    out_snapshot->revision = results->revision;
    out_snapshot->has_selection = results->selected_index < results->count;
    return UMI_STATUS_OK;
}

size_t umi_editor_reference_results_group_count(
    const UmiEditorReferenceResults *results)
{
    return results != NULL ? results->group_count : 0U;
}

size_t umi_editor_reference_results_count(
    const UmiEditorReferenceResults *results)
{
    return results != NULL ? results->count : 0U;
}

size_t umi_editor_reference_results_visible_count(
    const UmiEditorReferenceResults *results)
{
    size_t count = 0U;
    size_t index;
    if (results == NULL) return 0U;
    for (index = 0U; index < results->count; ++index) {
        if (results->entries[index].visible) count += 1U;
    }
    return count;
}

uint64_t umi_editor_reference_results_revision(
    const UmiEditorReferenceResults *results)
{
    return results != NULL ? results->revision : 0U;
}
