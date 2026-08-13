/*-----------------------------------------------------------------------------
 * Umicom Framework terminal event journal.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/event.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

struct UmiTerminalEventLog {
    UmiTerminalEvent *events;
    size_t capacity;
    size_t count;
    size_t head;
    uint64_t next_sequence;
    uint64_t revision;
    UmiMutex *mutex;
};

UmiStatus umi_terminal_event_log_create(size_t capacity,
                                        UmiTerminalEventLog **out_log)
{
    UmiTerminalEventLog *log;
    if (out_log == NULL || capacity == 0U || capacity > UMI_TERMINAL_EVENT_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_log = NULL;
    log = (UmiTerminalEventLog *)calloc(1U, sizeof(*log));
    if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    log->events = (UmiTerminalEvent *)calloc(capacity, sizeof(*log->events));
    if (log->events == NULL || umi_mutex_create(&log->mutex) != UMI_STATUS_OK) {
        umi_terminal_event_log_destroy(log);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    log->capacity = capacity;
    log->next_sequence = 1U;
    *out_log = log;
    return UMI_STATUS_OK;
}

void umi_terminal_event_log_destroy(UmiTerminalEventLog *log)
{
    if (log == NULL) return;
    umi_mutex_destroy(log->mutex);
    free(log->events);
    free(log);
}

UmiStatus umi_terminal_event_log_append(UmiTerminalEventLog *log,
                                        const UmiTerminalEvent *event)
{
    size_t position;
    if (log == NULL || event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(log->mutex);
    position = (log->head + log->count) % log->capacity;
    if (log->count == log->capacity) {
        position = log->head;
        log->head = (log->head + 1U) % log->capacity;
    } else log->count += 1U;
    log->events[position] = *event;
    log->events[position].sequence = log->next_sequence++;
    log->revision += 1U;
    (void)umi_mutex_unlock(log->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_terminal_event_log_at(const UmiTerminalEventLog *log,
                                    size_t index,
                                    UmiTerminalEvent *out_event)
{
    if (log == NULL || out_event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(log->mutex);
    if (index >= log->count) {
        (void)umi_mutex_unlock(log->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_event = log->events[(log->head + index) % log->capacity];
    (void)umi_mutex_unlock(log->mutex);
    return UMI_STATUS_OK;
}

size_t umi_terminal_event_log_count(const UmiTerminalEventLog *log)
{
    size_t count;
    if (log == NULL) return 0U;
    (void)umi_mutex_lock(log->mutex);
    count = log->count;
    (void)umi_mutex_unlock(log->mutex);
    return count;
}

uint64_t umi_terminal_event_log_revision(const UmiTerminalEventLog *log)
{
    uint64_t revision;
    if (log == NULL) return 0U;
    (void)umi_mutex_lock(log->mutex);
    revision = log->revision;
    (void)umi_mutex_unlock(log->mutex);
    return revision;
}

void umi_terminal_event_log_clear(UmiTerminalEventLog *log)
{
    if (log == NULL) return;
    (void)umi_mutex_lock(log->mutex);
    log->count = 0U;
    log->head = 0U;
    log->revision += 1U;
    (void)memset(log->events, 0, log->capacity * sizeof(log->events[0]));
    (void)umi_mutex_unlock(log->mutex);
}
