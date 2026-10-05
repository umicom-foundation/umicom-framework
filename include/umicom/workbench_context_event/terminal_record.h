/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/terminal_record.h
 *
 * PURPOSE:
 *   Define the reusable terminal event record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_TERMINAL_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_TERMINAL_RECORD_H

#include "umicom/workbench_context_event/event.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event terminal record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventTerminalRecord {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_EVENT_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_EVENT_TEXT_CAPACITY];
    UmiWorkbenchContextEventKind event_kind;
    UmiContextKind context_kind;
    UmiWorkbenchContextEventPriority priority;
    UmiWorkbenchContextEventState state;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextEventTerminalRecord;

/**
 * Initialise workbench context event terminal record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_terminal_record_init(
    UmiWorkbenchContextEventTerminalRecord *record,
    const char *record_id);
/**
 * Check that workbench context event terminal record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_terminal_record_validate(
    const UmiWorkbenchContextEventTerminalRecord *record);
/**
 * Provide the workbench context event terminal record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_terminal_record_set_source(
    UmiWorkbenchContextEventTerminalRecord *record,
    const char *source_id);
/**
 * Provide the workbench context event terminal record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_terminal_record_set_subject(
    UmiWorkbenchContextEventTerminalRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context event terminal record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_terminal_record_set_group(
    UmiWorkbenchContextEventTerminalRecord *record,
    const char *group_id);
/**
 * Provide the workbench context event terminal record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_terminal_record_set_label(
    UmiWorkbenchContextEventTerminalRecord *record,
    const char *label);
/**
 * Provide the workbench context event terminal record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_terminal_record_hash(
    const UmiWorkbenchContextEventTerminalRecord *record);
/**
 * Provide the workbench context event terminal record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_terminal_record_touch(
    UmiWorkbenchContextEventTerminalRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_event_terminal_record_archive_encode(const UmiWorkbenchContextEventTerminalRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_event_terminal_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextEventTerminalRecord *value);

#ifdef __cplusplus
}
#endif
#endif
