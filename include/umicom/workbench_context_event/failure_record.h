/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/failure_record.h
 *
 * PURPOSE:
 *   Define the reusable event failure record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_FAILURE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_FAILURE_RECORD_H

#include "umicom/workbench_context_event/event.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event failure record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventFailureRecord {
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
} UmiWorkbenchContextEventFailureRecord;

/**
 * Initialise workbench context event failure record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_failure_record_init(
    UmiWorkbenchContextEventFailureRecord *record,
    const char *record_id);
/**
 * Check that workbench context event failure record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_failure_record_validate(
    const UmiWorkbenchContextEventFailureRecord *record);
/**
 * Provide the workbench context event failure record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_failure_record_set_source(
    UmiWorkbenchContextEventFailureRecord *record,
    const char *source_id);
/**
 * Provide the workbench context event failure record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_failure_record_set_subject(
    UmiWorkbenchContextEventFailureRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context event failure record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_failure_record_set_group(
    UmiWorkbenchContextEventFailureRecord *record,
    const char *group_id);
/**
 * Provide the workbench context event failure record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_failure_record_set_label(
    UmiWorkbenchContextEventFailureRecord *record,
    const char *label);
/**
 * Provide the workbench context event failure record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_failure_record_hash(
    const UmiWorkbenchContextEventFailureRecord *record);
/**
 * Provide the workbench context event failure record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_failure_record_touch(
    UmiWorkbenchContextEventFailureRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_event_failure_record_archive_encode(const UmiWorkbenchContextEventFailureRecord *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_event_failure_record_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextEventFailureRecord *value);

#ifdef __cplusplus
}
#endif
#endif
