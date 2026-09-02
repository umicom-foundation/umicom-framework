/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/retry_record.h
 *
 * PURPOSE:
 *   Define the reusable event retry record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_RETRY_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_RETRY_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event retry record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventRetryRecord {
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
} UmiWorkbenchContextEventRetryRecord;

/**
 * Initialise workbench context event retry record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_retry_record_init(
    UmiWorkbenchContextEventRetryRecord *record,
    const char *record_id);
/**
 * Check that workbench context event retry record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_retry_record_validate(
    const UmiWorkbenchContextEventRetryRecord *record);
/**
 * Provide the workbench context event retry record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_retry_record_set_source(
    UmiWorkbenchContextEventRetryRecord *record,
    const char *source_id);
/**
 * Provide the workbench context event retry record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_retry_record_set_subject(
    UmiWorkbenchContextEventRetryRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context event retry record set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_retry_record_set_group(
    UmiWorkbenchContextEventRetryRecord *record,
    const char *group_id);
/**
 * Provide the workbench context event retry record set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_retry_record_set_label(
    UmiWorkbenchContextEventRetryRecord *record,
    const char *label);
/**
 * Provide the workbench context event retry record hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_event_retry_record_hash(
    const UmiWorkbenchContextEventRetryRecord *record);
/**
 * Provide the workbench context event retry record touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_event_retry_record_touch(
    UmiWorkbenchContextEventRetryRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
