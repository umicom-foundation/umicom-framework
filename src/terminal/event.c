/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/event.c
 *
 * PURPOSE:
 *   Implement the event behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
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

/*
 * Initialise terminal event log from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_terminal_event_log_create(size_t capacity,
                                        UmiTerminalEventLog **out_log)
{
    UmiTerminalEventLog *log;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_log == NULL || capacity == 0U || capacity > UMI_TERMINAL_EVENT_MAX) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_log = NULL;
    log = (UmiTerminalEventLog *)calloc(1U, sizeof(*log));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    log->events = (UmiTerminalEvent *)calloc(capacity, sizeof(*log->events));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log->events == NULL || umi_mutex_create(&log->mutex) != UMI_STATUS_OK) {
        umi_terminal_event_log_destroy(log);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    log->capacity = capacity;
    log->next_sequence = 1U;
    *out_log = log;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by terminal event log so the same storage can be reused
 * safely.
 */
void umi_terminal_event_log_destroy(UmiTerminalEventLog *log)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return;
    umi_mutex_destroy(log->mutex);
    free(log->events);
    free(log);
}

/* Add terminal event log only after its inputs and available capacity have been checked. */
UmiStatus umi_terminal_event_log_append(UmiTerminalEventLog *log,
                                        const UmiTerminalEvent *event)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(log->mutex);
    position = (log->head + log->count) % log->capacity;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (log->count == log->capacity) {
        position = log->head;
        log->head = (log->head + 1U) % log->capacity;
    } /* Use this fallback path when the earlier condition does not apply. */ else log->count += 1U;
    log->events[position] = *event;
    log->events[position].sequence = log->next_sequence++;
    log->revision += 1U;
    (void)umi_mutex_unlock(log->mutex);
    return UMI_STATUS_OK;
}

/*
 * Find terminal event log while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_terminal_event_log_at(const UmiTerminalEventLog *log,
                                    size_t index,
                                    UmiTerminalEvent *out_event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || out_event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(log->mutex);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= log->count) {
        (void)umi_mutex_unlock(log->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_event = log->events[(log->head + index) % log->capacity];
    (void)umi_mutex_unlock(log->mutex);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by terminal event log without changing their
 * state.
 */
size_t umi_terminal_event_log_count(const UmiTerminalEventLog *log)
{
    size_t count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return 0U;
    (void)umi_mutex_lock(log->mutex);
    count = log->count;
    (void)umi_mutex_unlock(log->mutex);
    return count;
}

/*
 * Provide the terminal event log revision operation used by this module and its client
 * applications.
 */
uint64_t umi_terminal_event_log_revision(const UmiTerminalEventLog *log)
{
    uint64_t revision;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return 0U;
    (void)umi_mutex_lock(log->mutex);
    revision = log->revision;
    (void)umi_mutex_unlock(log->mutex);
    return revision;
}

/*
 * Release or reset state held by terminal event log so the same storage can be reused
 * safely.
 */
void umi_terminal_event_log_clear(UmiTerminalEventLog *log)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return;
    (void)umi_mutex_lock(log->mutex);
    log->count = 0U;
    log->head = 0U;
    log->revision += 1U;
    (void)memset(log->events, 0, log->capacity * sizeof(log->events[0]));
    (void)umi_mutex_unlock(log->mutex);
}
