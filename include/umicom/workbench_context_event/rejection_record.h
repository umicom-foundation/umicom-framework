/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/rejection_record.h
 *
 * PURPOSE:
 *   Define the reusable event rejection record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_REJECTION_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_REJECTION_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventRejectionRecord {
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
} UmiWorkbenchContextEventRejectionRecord;

void umi_workbench_context_event_rejection_record_init(
    UmiWorkbenchContextEventRejectionRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_rejection_record_validate(
    const UmiWorkbenchContextEventRejectionRecord *record);
UmiStatus umi_workbench_context_event_rejection_record_set_source(
    UmiWorkbenchContextEventRejectionRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_rejection_record_set_subject(
    UmiWorkbenchContextEventRejectionRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_rejection_record_set_group(
    UmiWorkbenchContextEventRejectionRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_rejection_record_set_label(
    UmiWorkbenchContextEventRejectionRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_rejection_record_hash(
    const UmiWorkbenchContextEventRejectionRecord *record);
void umi_workbench_context_event_rejection_record_touch(
    UmiWorkbenchContextEventRejectionRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
