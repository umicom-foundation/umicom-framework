/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/event.h
 *
 * PURPOSE:
 *   Define observable terminal/process/task events and a bounded event journal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_EVENT_H
#define UMICOM_TERMINAL_EVENT_H

#include "umicom/base/status.h"
#include "umicom/terminal/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the terminal event data shared with callers of this public contract.
 */
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

/**
 * Represent the terminal event log data shared with callers of this public contract.
 */
typedef struct UmiTerminalEventLog UmiTerminalEventLog;

/**
 * Initialise terminal event log from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_terminal_event_log_create(size_t capacity,
                                        UmiTerminalEventLog **out_log);
/**
 * Release or reset state held by terminal event log so the same storage can be reused
 * safely.
 */
void umi_terminal_event_log_destroy(UmiTerminalEventLog *log);
/**
 * Add terminal event log only after its inputs and available capacity have been checked.
 */
UmiStatus umi_terminal_event_log_append(UmiTerminalEventLog *log,
                                        const UmiTerminalEvent *event);
/**
 * Find terminal event log while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_terminal_event_log_at(const UmiTerminalEventLog *log,
                                    size_t index,
                                    UmiTerminalEvent *out_event);
/**
 * Return the number of records represented by terminal event log without changing their
 * state.
 */
size_t umi_terminal_event_log_count(const UmiTerminalEventLog *log);
/**
 * Provide the terminal event log revision operation used by this module and its client
 * applications.
 */
uint64_t umi_terminal_event_log_revision(const UmiTerminalEventLog *log);
/**
 * Release or reset state held by terminal event log so the same storage can be reused
 * safely.
 */
void umi_terminal_event_log_clear(UmiTerminalEventLog *log);

#ifdef __cplusplus
}
#endif
#endif
