/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/event.c
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
#include "umicom/diagnostics/event.h"

#include <stdlib.h>
#include <string.h>

#include "diagnostic_lock.h"

struct UmiDiagnosticEventStream {
    UmiDiagnosticEvent *items;
    size_t count;
    size_t head;
    uint64_t next_sequence;
    UmiDiagnosticEventSink sink;
    void *sink_data;
    UmiDiagnosticLock lock;
};

/*
 * Initialise diagnostic event stream from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_event_stream_create(UmiDiagnosticEventStream **out_stream)
{
    UmiDiagnosticEventStream *stream;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_stream == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_stream = NULL;
    stream = (UmiDiagnosticEventStream *)calloc(1U, sizeof(*stream));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    stream->items = (UmiDiagnosticEvent *)calloc(UMI_DIAGNOSTIC_EVENT_CAPACITY,
                                                 sizeof(*stream->items));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream->items == NULL) {
        free(stream);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    stream->next_sequence = 1U;
    umi_diagnostic_lock_init(&stream->lock);
    *out_stream = stream;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by diagnostic event stream so the same storage can be reused
 * safely.
 */
void umi_diagnostic_event_stream_destroy(UmiDiagnosticEventStream *stream)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream != NULL) {
        free(stream->items);
        free(stream);
    }
}

/*
 * Release or reset state held by diagnostic event stream so the same storage can be reused
 * safely.
 */
void umi_diagnostic_event_stream_clear(UmiDiagnosticEventStream *stream)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL) return;
    umi_diagnostic_lock_acquire(&stream->lock);
    (void)memset(stream->items, 0, UMI_DIAGNOSTIC_EVENT_CAPACITY * sizeof(*stream->items));
    stream->count = 0U;
    stream->head = 0U;
    umi_diagnostic_lock_release(&stream->lock);
}

/*
 * Add diagnostic event stream only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_diagnostic_event_stream_append(UmiDiagnosticEventStream *stream,
                                             const UmiDiagnosticEvent *event)
{
    UmiDiagnosticEvent stored;
    UmiDiagnosticEventSink sink;
    void *sink_data;
    size_t destination;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL || event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    stored = *event;
    stored.diagnostic_id[sizeof(stored.diagnostic_id) - 1U] = '\0';
    stored.channel_id[sizeof(stored.channel_id) - 1U] = '\0';
    stored.detail[sizeof(stored.detail) - 1U] = '\0';
    umi_diagnostic_lock_acquire(&stream->lock);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (stream->count < UMI_DIAGNOSTIC_EVENT_CAPACITY) {
        destination = (stream->head + stream->count) % UMI_DIAGNOSTIC_EVENT_CAPACITY;
        ++stream->count;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        destination = stream->head;
        stream->head = (stream->head + 1U) % UMI_DIAGNOSTIC_EVENT_CAPACITY;
    }
    stored.sequence = stream->next_sequence++;
    stream->items[destination] = stored;
    sink = stream->sink;
    sink_data = stream->sink_data;
    umi_diagnostic_lock_release(&stream->lock);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (sink != NULL) sink(&stored, sink_data);
    return UMI_STATUS_OK;
}

/*
 * Find diagnostic event stream while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_event_stream_at(const UmiDiagnosticEventStream *stream,
                                         size_t index,
                                         UmiDiagnosticEvent *out_event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL || out_event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&stream->lock);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= stream->count) {
        umi_diagnostic_lock_release(&stream->lock);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_event = stream->items[(stream->head + index) % UMI_DIAGNOSTIC_EVENT_CAPACITY];
    umi_diagnostic_lock_release(&stream->lock);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by diagnostic event stream without changing
 * their state.
 */
size_t umi_diagnostic_event_stream_count(const UmiDiagnosticEventStream *stream)
{
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream != NULL) {
        umi_diagnostic_lock_acquire(&stream->lock);
        count = stream->count;
        umi_diagnostic_lock_release(&stream->lock);
    }
    return count;
}

/*
 * Provide the diagnostic event stream set sink operation used by this module and its
 * client applications.
 */
void umi_diagnostic_event_stream_set_sink(UmiDiagnosticEventStream *stream,
                                          UmiDiagnosticEventSink sink,
                                          void *user_data)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL) return;
    umi_diagnostic_lock_acquire(&stream->lock);
    stream->sink = sink;
    stream->sink_data = user_data;
    umi_diagnostic_lock_release(&stream->lock);
}

/*
 * Provide the diagnostic event kind text operation used by this module and its client
 * applications.
 */
const char *umi_diagnostic_event_kind_text(UmiDiagnosticEventKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
        case UMI_DIAGNOSTIC_EVENT_OUTPUT_ACCEPTED: return "OUTPUT_ACCEPTED";
        case UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_CREATED: return "DIAGNOSTIC_CREATED";
        case UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_UPDATED: return "DIAGNOSTIC_UPDATED";
        case UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_RESOLVED: return "DIAGNOSTIC_RESOLVED";
        case UMI_DIAGNOSTIC_EVENT_CLEARED: return "CLEARED";
        case UMI_DIAGNOSTIC_EVENT_DROPPED: return "DROPPED";
        default: return "UNKNOWN";
    }
}
