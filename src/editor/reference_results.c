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

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
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
    if (destination == NULL || source == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the reserve groups operation used by this module and its client applications. */
static UmiStatus reserve_groups(UmiEditorReferenceResults *results,
                                size_t required)
{
    size_t capacity;
    UmiEditorReferenceGroup *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= results->group_capacity) return UMI_STATUS_OK;
    capacity = results->group_capacity > 0U ? results->group_capacity : 16U;
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
    replacement = (UmiEditorReferenceGroup *)realloc(
        results->groups, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    results->groups = replacement;
    results->group_capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the reserve entries operation used by this module and its client applications. */
static UmiStatus reserve_entries(UmiEditorReferenceResults *results,
                                 size_t required)
{
    size_t capacity;
    UmiEditorReferenceEntry *replacement;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (required <= results->entry_capacity) return UMI_STATUS_OK;
    capacity = results->entry_capacity > 0U ? results->entry_capacity : 64U;
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
    replacement = (UmiEditorReferenceEntry *)realloc(
        results->entries, capacity * sizeof(*replacement));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (replacement == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    results->entries = replacement;
    results->entry_capacity = capacity;
    return UMI_STATUS_OK;
}

/* Provide the find group operation used by this module and its client applications. */
static size_t find_group(const UmiEditorReferenceResults *results,
                         const char *uri)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < results->group_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(results->groups[index].uri, uri) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the file label operation used by this module and its client applications. */
static const char *file_label(const char *uri)
{
    const char *cursor;
    const char *label = uri;
    /* Visit each bounded item once so every record receives the same rule. */
    for (cursor = uri; *cursor != '\0'; ++cursor) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '/' || *cursor == '\\') label = cursor + 1;
    }
    return label[0] != '\0' ? label : uri;
}

/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *needle)
{
    const char *start;
    /* Apply this branch only when its contract condition is satisfied. */
    if (needle[0] == '\0') return 1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (start = text; *start != '\0'; ++start) {
        const char *left = start;
        const char *right = needle;
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (*left != '\0' && *right != '\0' &&
               tolower((unsigned char)*left) ==
                   tolower((unsigned char)*right)) {
            left += 1;
            right += 1;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (*right == '\0') return 1;
    }
    return 0;
}

/* Provide the entry matches operation used by this module and its client applications. */
static int entry_matches(const UmiEditorReferenceEntry *entry,
                         const char *filter)
{
    return contains_case_insensitive(entry->result.location.uri, filter) ||
           contains_case_insensitive(entry->result.location.label, filter) ||
           contains_case_insensitive(entry->result.location.preview, filter) ||
           contains_case_insensitive(entry->result.detail, filter);
}

/*
 * Provide the refresh visibility operation used by this module and its client
 * applications.
 */
static void refresh_visibility(UmiEditorReferenceResults *results)
{
    size_t group_index;
    size_t entry_index;
    size_t first_visible = SIZE_MAX;

    /* Visit each bounded item once so every record receives the same rule. */
    for (group_index = 0U; group_index < results->group_count; ++group_index) {
        results->groups[group_index].visible_result_count = 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (entry_index = 0U; entry_index < results->count; ++entry_index) {
        UmiEditorReferenceEntry *entry = &results->entries[entry_index];
        UmiEditorReferenceGroup *group = &results->groups[entry->group_index];
        entry->visible = group->expanded &&
            entry_matches(entry, results->filter);
        /* Apply this operation only while the related capability or state is available. */
        if (entry->visible) {
            group->visible_result_count += 1U;
            /* Apply this operation only while the related capability or state is available. */
            if (first_visible == SIZE_MAX) first_visible = entry_index;
        }
        entry->selected = 0;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (results->selected_index >= results->count ||
        !results->entries[results->selected_index].visible) {
        results->selected_index = first_visible;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (results->selected_index < results->count) {
        results->entries[results->selected_index].selected = 1;
    }
}

/*
 * Initialise editor reference results from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_reference_results_create(
    UmiEditorReferenceResults **out_results)
{
    UmiEditorReferenceResults *results;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_results = NULL;
    results = (UmiEditorReferenceResults *)calloc(1U, sizeof(*results));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    results->selected_index = SIZE_MAX;
    results->revision = 1U;
    *out_results = results;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by editor reference results so the same storage can be
 * reused safely.
 */
void umi_editor_reference_results_destroy(UmiEditorReferenceResults *results)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL) return;
    free(results->entries);
    free(results->groups);
    results->entries = NULL;
    results->groups = NULL;
    free(results);
}

/*
 * Read editor reference results into validated module state and return a status when input
 * cannot be used.
 */
UmiStatus umi_editor_reference_results_load(
    UmiEditorReferenceResults *results,
    const UmiEditorNavigationResultSet *navigation_results)
{
    size_t count;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL || navigation_results == NULL ||
        umi_editor_navigation_result_set_query_kind(navigation_results) !=
            UMI_EDITOR_NAVIGATION_QUERY_REFERENCE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = umi_editor_navigation_result_set_count(navigation_results);
    status = reserve_entries(results, count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    results->count = 0U;
    results->group_count = 0U;
    results->selected_index = SIZE_MAX;
    results->filter[0] = '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiEditorNavigationResult navigation_result;
        UmiEditorReferenceEntry entry = {0};
        size_t group_index;

        status = umi_editor_navigation_result_set_at(navigation_results,
                                                      index,
                                                      &navigation_result);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        group_index = find_group(results, navigation_result.location.uri);
        /* Apply this branch only when its contract condition is satisfied. */
        if (group_index == SIZE_MAX) {
            UmiEditorReferenceGroup group = {0};
            status = reserve_groups(results, results->group_count + 1U);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            group.struct_size = (uint32_t)sizeof(group);
            group.api_version = UMI_EDITOR_REFERENCE_RESULTS_API_VERSION;
            group.expanded = 1;
            status = copy_text(group.uri, sizeof(group.uri),
                               navigation_result.location.uri);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) {
                status = copy_text(group.label,
                                   sizeof(group.label),
                                   file_label(navigation_result.location.uri));
            }
            /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the editor reference results set filter operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_reference_results_set_filter(
    UmiEditorReferenceResults *results,
    const char *filter)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL || filter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = copy_text(results->filter, sizeof(results->filter), filter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    refresh_visibility(results);
    results->revision = next_revision(results->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor reference results set group expanded operation used by this module
 * and its client applications.
 */
UmiStatus umi_editor_reference_results_set_group_expanded(
    UmiEditorReferenceResults *results,
    size_t group_index,
    int expanded)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (group_index >= results->group_count) return UMI_STATUS_NOT_FOUND;
    results->groups[group_index].expanded = expanded != 0;
    refresh_visibility(results);
    results->revision = next_revision(results->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the editor reference results select operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_reference_results_select(
    UmiEditorReferenceResults *results,
    size_t result_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (result_index >= results->count ||
        !results->entries[result_index].visible) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (results->selected_index < results->count) {
        results->entries[results->selected_index].selected = 0;
    }
    results->selected_index = result_index;
    results->entries[result_index].selected = 1;
    results->revision = next_revision(results->revision);
    return UMI_STATUS_OK;
}

/* Provide the select relative operation used by this module and its client applications. */
static UmiStatus select_relative(UmiEditorReferenceResults *results,
                                 int forward,
                                 int wrap)
{
    size_t index;
    size_t visited;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (results->count == 0U) return UMI_STATUS_NOT_FOUND;
    index = results->selected_index < results->count
        ? results->selected_index
        : (forward ? results->count - 1U : 0U);
    /* Visit each bounded item once so every record receives the same rule. */
    for (visited = 0U; visited < results->count; ++visited) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (forward) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1U < results->count) index += 1U;
            else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (wrap) index = 0U;
            /* Use this fallback path when the earlier condition does not apply. */
            else return UMI_STATUS_NOT_FOUND;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index > 0U) index -= 1U;
            else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (wrap) index = results->count - 1U;
            /* Use this fallback path when the earlier condition does not apply. */
            else return UMI_STATUS_NOT_FOUND;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (results->entries[index].visible) {
            return umi_editor_reference_results_select(results, index);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the editor reference results select next operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_reference_results_select_next(
    UmiEditorReferenceResults *results,
    int wrap)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return select_relative(results, 1, wrap);
}

/*
 * Provide the editor reference results select previous operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_reference_results_select_previous(
    UmiEditorReferenceResults *results,
    int wrap)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return select_relative(results, 0, wrap);
}

/*
 * Find editor reference results group while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_reference_results_group_at(
    const UmiEditorReferenceResults *results,
    size_t group_index,
    UmiEditorReferenceGroup *out_group)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL || out_group == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (group_index >= results->group_count) return UMI_STATUS_NOT_FOUND;
    *out_group = results->groups[group_index];
    return UMI_STATUS_OK;
}

/*
 * Find editor reference results entry while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_editor_reference_results_entry_at(
    const UmiEditorReferenceResults *results,
    size_t result_index,
    UmiEditorReferenceEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (result_index >= results->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = results->entries[result_index];
    return UMI_STATUS_OK;
}

/*
 * Find editor reference results visible entry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_reference_results_visible_entry_at(
    const UmiEditorReferenceResults *results,
    size_t visible_index,
    UmiEditorReferenceEntry *out_entry)
{
    size_t index;
    size_t current = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < results->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!results->entries[index].visible) continue;
        /* Apply this operation only while the related capability or state is available. */
        if (current == visible_index) {
            *out_entry = results->entries[index];
            return UMI_STATUS_OK;
        }
        current += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find editor reference results while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_editor_reference_results_selected(
    const UmiEditorReferenceResults *results,
    UmiEditorReferenceEntry *out_entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (results->selected_index >= results->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = results->entries[results->selected_index];
    return UMI_STATUS_OK;
}

/*
 * Provide the editor reference results snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_reference_results_snapshot(
    const UmiEditorReferenceResults *results,
    UmiEditorReferenceResultsSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Return the number of records represented by editor reference results group without
 * changing their state.
 */
size_t umi_editor_reference_results_group_count(
    const UmiEditorReferenceResults *results)
{
    return results != NULL ? results->group_count : 0U;
}

/*
 * Return the number of records represented by editor reference results without changing
 * their state.
 */
size_t umi_editor_reference_results_count(
    const UmiEditorReferenceResults *results)
{
    return results != NULL ? results->count : 0U;
}

/*
 * Return the number of records represented by editor reference results visible without
 * changing their state.
 */
size_t umi_editor_reference_results_visible_count(
    const UmiEditorReferenceResults *results)
{
    size_t count = 0U;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (results == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < results->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (results->entries[index].visible) count += 1U;
    }
    return count;
}

/*
 * Provide the editor reference results revision operation used by this module and its
 * client applications.
 */
uint64_t umi_editor_reference_results_revision(
    const UmiEditorReferenceResults *results)
{
    return results != NULL ? results->revision : 0U;
}
