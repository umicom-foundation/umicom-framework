/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/consumer_record.h
 *
 * PURPOSE:
 *   Define the reusable event consumer record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_CONSUMER_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_CONSUMER_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event consumer record data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventConsumerRecord {
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
} UmiWorkbenchContextEventConsumerRecord;

/**
 * Initialise workbench context event consumer record from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_consumer_record_init(
    UmiWorkbenchContextEventConsumerRecord *record,
    const char *record_id);
/**
 * Check that workbench context event consumer record satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_consumer_record_validate(
    const UmiWorkbenchContextEventConsumerRecord *record);
/**
 * Provide the workbench context event consumer record set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_consumer_record_set_source(
    UmiWorkbenchContextEventConsumerRecord *record,
    const char *source_id);
/**
 * Provide the workbench context event consumer record set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_consumer_record_set_subject(
    UmiWorkbenchContextEventConsumerRecord *record,
    const char *subject_id);
/**
 * Provide the workbench context event consumer record set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_consumer_record_set_group(
    UmiWorkbenchContextEventConsumerRecord *record,
    const char *group_id);
/**
 * Provide the workbench context event consumer record set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_consumer_record_set_label(
    UmiWorkbenchContextEventConsumerRecord *record,
    const char *label);
/**
 * Provide the workbench context event consumer record hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_consumer_record_hash(
    const UmiWorkbenchContextEventConsumerRecord *record);
/**
 * Provide the workbench context event consumer record touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_consumer_record_touch(
    UmiWorkbenchContextEventConsumerRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
