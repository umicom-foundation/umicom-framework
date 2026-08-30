/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/layout_history.c
 *
 * PURPOSE:
 *   Implement bounded full-workspace transactions above the existing visual
 *   layout designer. This is the shared history authority for all applications
 *   and never calls application code during restoration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/layout_history.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiDesktopLayoutHistoryEntry {
    UmiDesktopLayoutHistoryEntrySnapshot snapshot;
    UmiDesktopRuntimeState before;
    UmiDesktopRuntimeState after;
} UmiDesktopLayoutHistoryEntry;

struct UmiDesktopLayoutHistory {
    UmiDesktopRuntime *runtime;
    UmiDesktopLayoutHistoryEntry entries[UMI_DESKTOP_LAYOUT_HISTORY_MAX];
    UmiDesktopLayoutHistoryEntry pending;
    size_t count;
    size_t cursor;
    bool transaction_open;
    uint64_t last_sequence;
    uint64_t revision;
};

static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static bool same_transaction(
    const UmiDesktopLayoutHistoryEntry *entry,
    const char *transaction_id)
{
    return entry != NULL && transaction_id != NULL &&
        strcmp(entry->snapshot.transaction_id, transaction_id) == 0;
}

UmiStatus umi_desktop_layout_history_create(
    UmiDesktopRuntime *runtime,
    UmiDesktopLayoutHistory **out_history)
{
    UmiDesktopLayoutHistory *history;
    if (runtime == NULL || out_history == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;
    history = (UmiDesktopLayoutHistory *)calloc(1U, sizeof(*history));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    history->runtime = runtime;
    history->revision = 1U;
    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_desktop_layout_history_destroy(UmiDesktopLayoutHistory *history)
{
    free(history);
}

UmiStatus umi_desktop_layout_history_begin(
    UmiDesktopLayoutHistory *history,
    const char *transaction_id,
    const char *operation_id,
    const char *source_application_id,
    const char *label)
{
    UmiStatus status;
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->transaction_open) return UMI_STATUS_INVALID_STATE;
    (void)memset(&history->pending, 0, sizeof(history->pending));
    history->pending.snapshot.structure_size =
        (uint32_t)sizeof(history->pending.snapshot);
    status = copy_text(history->pending.snapshot.transaction_id,
                       sizeof(history->pending.snapshot.transaction_id),
                       transaction_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(history->pending.snapshot.operation_id,
                           sizeof(history->pending.snapshot.operation_id),
                           operation_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(
            history->pending.snapshot.source_application_id,
            sizeof(history->pending.snapshot.source_application_id),
            source_application_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(history->pending.snapshot.label,
                           sizeof(history->pending.snapshot.label), label);
    if (status == UMI_STATUS_OK)
        status = umi_desktop_runtime_capture_state(
            history->runtime, &history->pending.before);
    if (status != UMI_STATUS_OK) {
        (void)memset(&history->pending, 0, sizeof(history->pending));
        return status;
    }
    history->pending.snapshot.sequence = history->last_sequence + 1U;
    history->pending.snapshot.state = UMI_DESKTOP_LAYOUT_HISTORY_OPEN;
    history->pending.snapshot.revision = 1U;
    history->transaction_open = true;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

static void discard_redo(UmiDesktopLayoutHistory *history)
{
    if (history->cursor >= history->count) return;
    (void)memset(&history->entries[history->cursor], 0,
                 (history->count - history->cursor) *
                     sizeof(history->entries[0]));
    history->count = history->cursor;
}

static void append_pending(UmiDesktopLayoutHistory *history)
{
    discard_redo(history);
    if (history->count == UMI_DESKTOP_LAYOUT_HISTORY_MAX) {
        (void)memmove(&history->entries[0], &history->entries[1],
                      (UMI_DESKTOP_LAYOUT_HISTORY_MAX - 1U) *
                          sizeof(history->entries[0]));
        history->count -= 1U;
        if (history->cursor > 0U) history->cursor -= 1U;
    }
    history->entries[history->count] = history->pending;
    history->count += 1U;
    history->cursor = history->count;
}

UmiStatus umi_desktop_layout_history_commit(
    UmiDesktopLayoutHistory *history,
    const char *transaction_id)
{
    UmiStatus status;
    if (history == NULL || transaction_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!history->transaction_open ||
        !same_transaction(&history->pending, transaction_id))
        return UMI_STATUS_INVALID_STATE;
    status = umi_desktop_runtime_capture_state(
        history->runtime, &history->pending.after);
    if (status != UMI_STATUS_OK) return status;
    history->pending.snapshot.state = UMI_DESKTOP_LAYOUT_HISTORY_COMMITTED;
    history->pending.snapshot.applied = true;
    history->pending.snapshot.revision += 1U;
    history->last_sequence = history->pending.snapshot.sequence;
    append_pending(history);
    (void)memset(&history->pending, 0, sizeof(history->pending));
    history->transaction_open = false;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_history_rollback(
    UmiDesktopLayoutHistory *history,
    const char *transaction_id)
{
    UmiStatus status;
    if (history == NULL || transaction_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!history->transaction_open ||
        !same_transaction(&history->pending, transaction_id))
        return UMI_STATUS_INVALID_STATE;
    status = umi_desktop_runtime_restore_state(
        history->runtime, &history->pending.before);
    if (status != UMI_STATUS_OK) return status;
    history->pending.snapshot.state =
        UMI_DESKTOP_LAYOUT_HISTORY_ROLLED_BACK;
    (void)memset(&history->pending, 0, sizeof(history->pending));
    history->transaction_open = false;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_history_undo(UmiDesktopLayoutHistory *history)
{
    UmiDesktopLayoutHistoryEntry *entry;
    UmiStatus status;
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->transaction_open || history->cursor == 0U)
        return UMI_STATUS_INVALID_STATE;
    entry = &history->entries[history->cursor - 1U];
    status = umi_desktop_runtime_restore_state(
        history->runtime, &entry->before);
    if (status != UMI_STATUS_OK) return status;
    history->cursor -= 1U;
    entry->snapshot.state = UMI_DESKTOP_LAYOUT_HISTORY_UNDONE;
    entry->snapshot.applied = false;
    entry->snapshot.revision += 1U;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_history_redo(UmiDesktopLayoutHistory *history)
{
    UmiDesktopLayoutHistoryEntry *entry;
    UmiStatus status;
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->transaction_open || history->cursor >= history->count)
        return UMI_STATUS_INVALID_STATE;
    entry = &history->entries[history->cursor];
    status = umi_desktop_runtime_restore_state(
        history->runtime, &entry->after);
    if (status != UMI_STATUS_OK) return status;
    history->cursor += 1U;
    entry->snapshot.state = UMI_DESKTOP_LAYOUT_HISTORY_COMMITTED;
    entry->snapshot.applied = true;
    entry->snapshot.revision += 1U;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_history_entry_at(
    const UmiDesktopLayoutHistory *history,
    size_t index,
    UmiDesktopLayoutHistoryEntrySnapshot *out_entry)
{
    if (history == NULL || out_entry == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= history->count) return UMI_STATUS_NOT_FOUND;
    *out_entry = history->entries[index].snapshot;
    out_entry->applied = index < history->cursor;
    return UMI_STATUS_OK;
}

UmiStatus umi_desktop_layout_history_snapshot(
    const UmiDesktopLayoutHistory *history,
    UmiDesktopLayoutHistorySnapshot *out_snapshot)
{
    if (history == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->entry_count = history->count;
    out_snapshot->applied_count = history->cursor;
    out_snapshot->transaction_open = history->transaction_open;
    out_snapshot->can_undo = !history->transaction_open &&
        history->cursor > 0U;
    out_snapshot->can_redo = !history->transaction_open &&
        history->cursor < history->count;
    out_snapshot->last_sequence = history->last_sequence;
    out_snapshot->revision = history->revision;
    if (history->transaction_open) {
        (void)memcpy(out_snapshot->pending_transaction_id,
                     history->pending.snapshot.transaction_id,
                     sizeof(out_snapshot->pending_transaction_id));
    }
    return UMI_STATUS_OK;
}
