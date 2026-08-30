/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/navigation_history.c
 *
 * PURPOSE:
 *   Implement source navigation history as a bounded linear timeline. Moving
 *   back and recording a new location intentionally discards the old forward
 *   branch, matching professional editor and browser behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/navigation_history.h"

#include <stdlib.h>
#include <string.h>

struct UmiEditorNavigationHistory {
    UmiEditorSourceLocation *entries;
    size_t count;
    size_t current_index;
    size_t capacity;
    uint64_t revision;
};

static uint64_t next_revision(uint64_t revision)
{
    return revision == UINT64_MAX ? 1U : revision + 1U;
}

UmiStatus umi_editor_navigation_history_create(
    size_t capacity,
    UmiEditorNavigationHistory **out_history)
{
    UmiEditorNavigationHistory *history;

    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;
    if (capacity == 0U) {
        capacity = UMI_EDITOR_NAVIGATION_HISTORY_DEFAULT_CAPACITY;
    }
    if (capacity > SIZE_MAX / sizeof(*history->entries)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    history = (UmiEditorNavigationHistory *)calloc(1U, sizeof(*history));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    history->entries = (UmiEditorSourceLocation *)calloc(
        capacity, sizeof(*history->entries));
    if (history->entries == NULL) {
        free(history);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    history->capacity = capacity;
    history->revision = 1U;
    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_editor_navigation_history_destroy(
    UmiEditorNavigationHistory *history)
{
    if (history == NULL) return;
    free(history->entries);
    history->entries = NULL;
    free(history);
}

UmiStatus umi_editor_navigation_history_record(
    UmiEditorNavigationHistory *history,
    const UmiEditorSourceLocation *location)
{
    if (history == NULL ||
        umi_editor_source_location_validate(location) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (history->count > 0U &&
        umi_editor_source_location_same_position(
            &history->entries[history->current_index], location)) {
        history->entries[history->current_index] = *location;
        history->revision = next_revision(history->revision);
        return UMI_STATUS_OK;
    }

    if (history->count > 0U &&
        history->current_index + 1U < history->count) {
        history->count = history->current_index + 1U;
    }
    if (history->count == history->capacity) {
        (void)memmove(history->entries,
                      history->entries + 1U,
                      (history->capacity - 1U) * sizeof(*history->entries));
        history->count -= 1U;
    }
    history->entries[history->count] = *location;
    history->count += 1U;
    history->current_index = history->count - 1U;
    history->revision = next_revision(history->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_history_replace_current(
    UmiEditorNavigationHistory *history,
    const UmiEditorSourceLocation *location)
{
    if (history == NULL || history->count == 0U ||
        umi_editor_source_location_validate(location) != UMI_STATUS_OK) {
        return history != NULL && history->count == 0U
            ? UMI_STATUS_NOT_FOUND
            : UMI_STATUS_INVALID_ARGUMENT;
    }
    history->entries[history->current_index] = *location;
    history->revision = next_revision(history->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_history_current(
    const UmiEditorNavigationHistory *history,
    UmiEditorSourceLocation *out_location)
{
    if (history == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (history->count == 0U) return UMI_STATUS_NOT_FOUND;
    *out_location = history->entries[history->current_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_history_go_back(
    UmiEditorNavigationHistory *history,
    UmiEditorSourceLocation *out_location)
{
    if (history == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (history->count == 0U || history->current_index == 0U) {
        return UMI_STATUS_NOT_FOUND;
    }
    history->current_index -= 1U;
    history->revision = next_revision(history->revision);
    *out_location = history->entries[history->current_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_history_go_forward(
    UmiEditorNavigationHistory *history,
    UmiEditorSourceLocation *out_location)
{
    if (history == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (history->count == 0U ||
        history->current_index + 1U >= history->count) {
        return UMI_STATUS_NOT_FOUND;
    }
    history->current_index += 1U;
    history->revision = next_revision(history->revision);
    *out_location = history->entries[history->current_index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_history_at(
    const UmiEditorNavigationHistory *history,
    size_t index,
    UmiEditorSourceLocation *out_location)
{
    if (history == NULL || out_location == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= history->count) return UMI_STATUS_NOT_FOUND;
    *out_location = history->entries[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_history_clear(
    UmiEditorNavigationHistory *history)
{
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    history->count = 0U;
    history->current_index = 0U;
    history->revision = next_revision(history->revision);
    return UMI_STATUS_OK;
}

UmiStatus umi_editor_navigation_history_snapshot(
    const UmiEditorNavigationHistory *history,
    UmiEditorNavigationHistorySnapshot *out_snapshot)
{
    if (history == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_EDITOR_NAVIGATION_HISTORY_API_VERSION;
    out_snapshot->count = history->count;
    out_snapshot->current_index = history->count > 0U
        ? history->current_index
        : 0U;
    out_snapshot->capacity = history->capacity;
    out_snapshot->revision = history->revision;
    out_snapshot->has_current = history->count > 0U;
    out_snapshot->can_go_back = history->count > 0U &&
        history->current_index > 0U;
    out_snapshot->can_go_forward = history->count > 0U &&
        history->current_index + 1U < history->count;
    return UMI_STATUS_OK;
}
