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

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_event_failure_record_init(
    UmiWorkbenchContextEventFailureRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_failure_record_validate(
    const UmiWorkbenchContextEventFailureRecord *record);
UmiStatus umi_workbench_context_event_failure_record_set_source(
    UmiWorkbenchContextEventFailureRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_failure_record_set_subject(
    UmiWorkbenchContextEventFailureRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_failure_record_set_group(
    UmiWorkbenchContextEventFailureRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_failure_record_set_label(
    UmiWorkbenchContextEventFailureRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_failure_record_hash(
    const UmiWorkbenchContextEventFailureRecord *record);
void umi_workbench_context_event_failure_record_touch(
    UmiWorkbenchContextEventFailureRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
