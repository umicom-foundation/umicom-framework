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

typedef enum UmiDiagnosticEventKind {
    UMI_DIAGNOSTIC_EVENT_OUTPUT_ACCEPTED = 0,
    UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_CREATED = 1,
    UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_UPDATED = 2,
    UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_RESOLVED = 3,
    UMI_DIAGNOSTIC_EVENT_CLEARED = 4,
    UMI_DIAGNOSTIC_EVENT_DROPPED = 5
} UmiDiagnosticEventKind;

typedef struct UmiDiagnosticEvent {
    UmiDiagnosticEventKind kind;
    char diagnostic_id[UMI_DIAGNOSTIC_ID_CAPACITY];
    char channel_id[128];
    char detail[512];
    uint64_t sequence;
    uint64_t timestamp_ns;
    uint64_t correlation_id;
} UmiDiagnosticEvent;

typedef struct UmiDiagnosticEventStream UmiDiagnosticEventStream;
typedef void (*UmiDiagnosticEventSink)(const UmiDiagnosticEvent *event,
                                       void *user_data);

UmiStatus umi_diagnostic_event_stream_create(
    UmiDiagnosticEventStream **out_stream);
void umi_diagnostic_event_stream_destroy(UmiDiagnosticEventStream *stream);
void umi_diagnostic_event_stream_clear(UmiDiagnosticEventStream *stream);
UmiStatus umi_diagnostic_event_stream_append(UmiDiagnosticEventStream *stream,
                                             const UmiDiagnosticEvent *event);
UmiStatus umi_diagnostic_event_stream_at(const UmiDiagnosticEventStream *stream,
                                         size_t index,
                                         UmiDiagnosticEvent *out_event);
size_t umi_diagnostic_event_stream_count(const UmiDiagnosticEventStream *stream);
void umi_diagnostic_event_stream_set_sink(UmiDiagnosticEventStream *stream,
                                          UmiDiagnosticEventSink sink,
                                          void *user_data);
const char *umi_diagnostic_event_kind_text(UmiDiagnosticEventKind kind);

#ifdef __cplusplus
}
#endif
#endif
