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

void umi_workbench_context_event_backpressure_record_init(
    UmiWorkbenchContextEventBackpressureRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_backpressure_record_validate(
    const UmiWorkbenchContextEventBackpressureRecord *record);
UmiStatus umi_workbench_context_event_backpressure_record_set_source(
    UmiWorkbenchContextEventBackpressureRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_backpressure_record_set_subject(
    UmiWorkbenchContextEventBackpressureRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_backpressure_record_set_group(
    UmiWorkbenchContextEventBackpressureRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_backpressure_record_set_label(
    UmiWorkbenchContextEventBackpressureRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_backpressure_record_hash(
    const UmiWorkbenchContextEventBackpressureRecord *record);
void umi_workbench_context_event_backpressure_record_touch(
    UmiWorkbenchContextEventBackpressureRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
