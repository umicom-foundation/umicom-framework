/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/backpressure_record.h
 *
 * PURPOSE:
 *   Define the reusable event backpressure record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_BACKPRESSURE_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_BACKPRESSURE_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event backpressure record data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextEventBackpressureRecord {
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
} UmiWorkbenchContextEventBackpressureRecord;

/**
 * Initialise workbench context event backpressure record from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_event_backpressure_record_init(
    UmiWorkbenchContextEventBackpressureRecord *record,
    const char *record_id);
/**
 * Check that workbench context event backpressure record satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_event_backpressure_record_validate(
    const UmiWorkbenchContextEventBackpressureRecord *record);
/**
 * Provide the workbench context event backpressure record set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_event_backpressure_record_set_source(
    UmiWorkbenchContextEventBackpressureRecord *record,
    const char *source_id);
/**
 * Provide the workbench context event backpressure record set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_event_backpressure_record_set_subject(
    UmiWorkbenchContextEventBackpressureRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context event backpressure record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_backpressure_record_set_group(
    UmiWorkbenchContextEventBackpressureRecord *record,
    const char *group_id);
/**
 * Provide the workbench context event backpressure record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_backpressure_record_set_label(
    UmiWorkbenchContextEventBackpressureRecord *record,
    const char *label);
/**
 * Provide the workbench context event backpressure record hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_event_backpressure_record_hash(
    const UmiWorkbenchContextEventBackpressureRecord *record);
/**
 * Provide the workbench context event backpressure record touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_event_backpressure_record_touch(
    UmiWorkbenchContextEventBackpressureRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
