/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/producer_record.h
 *
 * PURPOSE:
 *   Define the reusable event producer record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_PRODUCER_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_PRODUCER_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventProducerRecord {
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
} UmiWorkbenchContextEventProducerRecord;

void umi_workbench_context_event_producer_record_init(
    UmiWorkbenchContextEventProducerRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_producer_record_validate(
    const UmiWorkbenchContextEventProducerRecord *record);
UmiStatus umi_workbench_context_event_producer_record_set_source(
    UmiWorkbenchContextEventProducerRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_producer_record_set_subject(
    UmiWorkbenchContextEventProducerRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_producer_record_set_group(
    UmiWorkbenchContextEventProducerRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_producer_record_set_label(
    UmiWorkbenchContextEventProducerRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_producer_record_hash(
    const UmiWorkbenchContextEventProducerRecord *record);
void umi_workbench_context_event_producer_record_touch(
    UmiWorkbenchContextEventProducerRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
