/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/event.h
 *
 * PURPOSE:
 *   Publish the public event contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_EVENT_H
#define UMICOM_DIAGNOSTICS_EVENT_H

#include "umicom/diagnostics/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DIAGNOSTIC_EVENT_CAPACITY 4096U

/**
 * List the named diagnostic event kind values accepted by this public contract.
 */
typedef enum UmiDiagnosticEventKind {
    UMI_DIAGNOSTIC_EVENT_OUTPUT_ACCEPTED = 0,
    UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_CREATED = 1,
    UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_UPDATED = 2,
    UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_RESOLVED = 3,
    UMI_DIAGNOSTIC_EVENT_CLEARED = 4,
    UMI_DIAGNOSTIC_EVENT_DROPPED = 5
} UmiDiagnosticEventKind;

/**
 * Represent the diagnostic event data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticEvent {
    UmiDiagnosticEventKind kind;
    char diagnostic_id[UMI_DIAGNOSTIC_ID_CAPACITY];
    char channel_id[128];
    char detail[512];
    uint64_t sequence;
    uint64_t timestamp_ns;
    uint64_t correlation_id;
} UmiDiagnosticEvent;

/**
 * Represent the diagnostic event stream data shared with callers of this public contract.
 */
typedef struct UmiDiagnosticEventStream UmiDiagnosticEventStream;
typedef void (*UmiDiagnosticEventSink)(const UmiDiagnosticEvent *event,
                                       void *user_data);

/**
 * Initialise diagnostic event stream from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_event_stream_create(
    UmiDiagnosticEventStream **out_stream);
/**
 * Release or reset state held by diagnostic event stream so the same storage can be reused
 * safely.
 */
void umi_diagnostic_event_stream_destroy(UmiDiagnosticEventStream *stream);
/**
 * Release or reset state held by diagnostic event stream so the same storage can be reused
 * safely.
 */
void umi_diagnostic_event_stream_clear(UmiDiagnosticEventStream *stream);
/**
 * Add diagnostic event stream only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_diagnostic_event_stream_append(UmiDiagnosticEventStream *stream,
                                             const UmiDiagnosticEvent *event);
/**
 * Find diagnostic event stream while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_diagnostic_event_stream_at(const UmiDiagnosticEventStream *stream,
                                         size_t index,
                                         UmiDiagnosticEvent *out_event);
/**
 * Return the number of records represented by diagnostic event stream without changing
 * their state.
 */
size_t umi_diagnostic_event_stream_count(const UmiDiagnosticEventStream *stream);
/**
 * Provide the diagnostic event stream set sink operation used by this module and its
 * client applications.
 */
void umi_diagnostic_event_stream_set_sink(UmiDiagnosticEventStream *stream,
                                          UmiDiagnosticEventSink sink,
                                          void *user_data);
/**
 * Provide the diagnostic event kind text operation used by this module and its client
 * applications.
 */
const char *umi_diagnostic_event_kind_text(UmiDiagnosticEventKind kind);

#ifdef __cplusplus
}
#endif
#endif
