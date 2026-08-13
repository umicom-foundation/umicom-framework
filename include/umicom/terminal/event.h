/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/event.h
 *
 * PURPOSE:
 *   Define observable terminal/process/task events and a bounded event journal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_EVENT_H
#define UMICOM_TERMINAL_EVENT_H

#include "umicom/base/status.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTerminalEvent {
    uint64_t sequence;
    uint64_t timestamp_ns;
    UmiTerminalEventKind kind;
    char subject_id[UMI_TERMINAL_ID_CAPACITY];
    char message[UMI_TERMINAL_LINE_CAPACITY];
    int status_code;
} UmiTerminalEvent;

typedef void (*UmiTerminalEventSink)(const UmiTerminalEvent *event,
                                     void *user_data);

typedef struct UmiTerminalEventLog UmiTerminalEventLog;

UmiStatus umi_terminal_event_log_create(size_t capacity,
                                        UmiTerminalEventLog **out_log);
void umi_terminal_event_log_destroy(UmiTerminalEventLog *log);
UmiStatus umi_terminal_event_log_append(UmiTerminalEventLog *log,
                                        const UmiTerminalEvent *event);
UmiStatus umi_terminal_event_log_at(const UmiTerminalEventLog *log,
                                    size_t index,
                                    UmiTerminalEvent *out_event);
size_t umi_terminal_event_log_count(const UmiTerminalEventLog *log);
uint64_t umi_terminal_event_log_revision(const UmiTerminalEventLog *log);
void umi_terminal_event_log_clear(UmiTerminalEventLog *log);

#ifdef __cplusplus
}
#endif
#endif
