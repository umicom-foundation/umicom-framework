/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/history.c
 *
 * PURPOSE:
 *   Implement a thread-safe command-history ring with search and navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/history.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

struct UmiTerminalHistory {
    UmiTerminalHistoryEntry *entries;
    size_t capacity;
    size_t count;
    size_t head;
    uint64_t next_sequence;
    uint64_t appended;
    uint64_t succeeded;
    uint64_t failed;
    uint64_t revision;
    UmiMutex *mutex;
};

static size_t physical_index(const UmiTerminalHistory *history, size_t index)
{
    return (history->head + index) % history->capacity;
}

UmiStatus umi_terminal_history_create(size_t capacity,
                                      UmiTerminalHistory **out_history)
{
    UmiTerminalHistory *history;
    if (out_history == NULL || capacity == 0U || capacity > UMI_TERMINAL_HISTORY_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_history = NULL;
    history = (UmiTerminalHistory *)calloc(1U, sizeof(*history));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    history->entries = (UmiTerminalHistoryEntry *)calloc(capacity,
                                                         sizeof(*history->entries));
    if (history->entries == NULL ||
        umi_mutex_create(&history->mutex) != UMI_STATUS_OK) {
        umi_terminal_history_destroy(history);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    history->capacity = capacity;
    history->next_sequence = 1U;
    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_terminal_history_destroy(UmiTerminalHistory *history)
{
    if (history == NULL) return;
    umi_mutex_destroy(history->mutex);
    free(history->entries);
    free(history);
}

UmiStatus umi_terminal_history_append(UmiTerminalHistory *history,
                                      const UmiTerminalHistoryEntry *entry)
{
    size_t position;
    if (history == NULL || entry == NULL || entry->command[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(history->mutex);
    if (history->count > 0U) {
        UmiTerminalHistoryEntry *last =
            &history->entries[physical_index(history, history->count - 1U)];
        if (strcmp(last->command, entry->command) == 0 &&
            strcmp(last->session_id, entry->session_id) == 0) {
            *last = *entry;
            last->sequence = history->next_sequence++;
            history->appended += 1U;
            history->succeeded += entry->completed && entry->exit_code == 0 ? 1U : 0U;
            history->failed += entry->completed && entry->exit_code != 0 ? 1U : 0U;
            history->revision += 1U;
            (void)umi_mutex_unlock(history->mutex);
            return UMI_STATUS_OK;
        }
    }
    position = (history->head + history->count) % history->capacity;
    if (history->count == history->capacity) {
        position = history->head;
        history->head = (history->head + 1U) % history->capacity;
    } else {
        history->count += 1U;
    }
    history->entries[position] = *entry;
    history->entries[position].sequence = history->next_sequence++;
    history->appended += 1U;
    history->succeeded += entry->completed && entry->exit_code == 0 ? 1U : 0U;
    history->failed += entry->completed && entry->exit_code != 0 ? 1U : 0U;
    history->revision += 1U;
    (void)umi_mutex_unlock(history->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_history_at(const UmiTerminalHistory *history,
                                  size_t index,
                                  UmiTerminalHistoryEntry *out_entry)
{
    if (history == NULL || out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(history->mutex);
    if (index >= history->count) {
        (void)umi_mutex_unlock(history->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_entry = history->entries[physical_index(history, index)];
    (void)umi_mutex_unlock(history->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_history_search(const UmiTerminalHistory *history,
                                      const char *query,
                                      size_t start_index,
                                      size_t *out_index,
                                      UmiTerminalHistoryEntry *out_entry)
{
    size_t offset;
    if (history == NULL || query == NULL || query[0] == '\0' ||
        out_index == NULL || out_entry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(history->mutex);
    if (history->count == 0U || start_index >= history->count) {
        (void)umi_mutex_unlock(history->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    for (offset = start_index + 1U; offset > 0U; --offset) {
        size_t index = offset - 1U;
        const UmiTerminalHistoryEntry *entry =
            &history->entries[physical_index(history, index)];
        if (strstr(entry->command, query) != NULL) {
            *out_index = index;
            *out_entry = *entry;
            (void)umi_mutex_unlock(history->mutex);
            return UMI_STATUS_OK;
        }
    }
    (void)umi_mutex_unlock(history->mutex);
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_terminal_history_previous(const UmiTerminalHistory *history,
                                        size_t *cursor,
                                        UmiTerminalHistoryEntry *out_entry)
{
    if (history == NULL || cursor == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(history->mutex);
    if (history->count == 0U) {
        (void)umi_mutex_unlock(history->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (*cursor > history->count) *cursor = history->count;
    if (*cursor == 0U) {
        (void)umi_mutex_unlock(history->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *cursor -= 1U;
    *out_entry = history->entries[physical_index(history, *cursor)];
    (void)umi_mutex_unlock(history->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_history_next(const UmiTerminalHistory *history,
                                    size_t *cursor,
                                    UmiTerminalHistoryEntry *out_entry)
{
    if (history == NULL || cursor == NULL || out_entry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(history->mutex);
    if (*cursor + 1U >= history->count) {
        *cursor = history->count;
        (void)umi_mutex_unlock(history->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *cursor += 1U;
    *out_entry = history->entries[physical_index(history, *cursor)];
    (void)umi_mutex_unlock(history->mutex);
    return UMI_STATUS_OK;
}

UmiTerminalHistoryStats umi_terminal_history_stats(
    const UmiTerminalHistory *history)
{
    UmiTerminalHistoryStats stats = {0};
    if (history == NULL) return stats;
    (void)umi_mutex_lock(history->mutex);
    stats.capacity = history->capacity;
    stats.count = history->count;
    stats.appended = history->appended;
    stats.succeeded = history->succeeded;
    stats.failed = history->failed;
    stats.revision = history->revision;
    (void)umi_mutex_unlock(history->mutex);
    return stats;
}

void umi_terminal_history_clear(UmiTerminalHistory *history)
{
    if (history == NULL) return;
    (void)umi_mutex_lock(history->mutex);
    history->count = 0U;
    history->head = 0U;
    history->revision += 1U;
    (void)memset(history->entries, 0,
                 history->capacity * sizeof(history->entries[0]));
    (void)umi_mutex_unlock(history->mutex);
}
