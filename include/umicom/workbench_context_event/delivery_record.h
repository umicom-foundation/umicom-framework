/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/delivery_record.h
 *
 * PURPOSE:
 *   Define the reusable event delivery record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_DELIVERY_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_DELIVERY_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventDeliveryRecord {
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
} UmiWorkbenchContextEventDeliveryRecord;

void umi_workbench_context_event_delivery_record_init(
    UmiWorkbenchContextEventDeliveryRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_delivery_record_validate(
    const UmiWorkbenchContextEventDeliveryRecord *record);
UmiStatus umi_workbench_context_event_delivery_record_set_source(
    UmiWorkbenchContextEventDeliveryRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_delivery_record_set_subject(
    UmiWorkbenchContextEventDeliveryRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_delivery_record_set_group(
    UmiWorkbenchContextEventDeliveryRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_delivery_record_set_label(
    UmiWorkbenchContextEventDeliveryRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_delivery_record_hash(
    const UmiWorkbenchContextEventDeliveryRecord *record);
void umi_workbench_context_event_delivery_record_touch(
    UmiWorkbenchContextEventDeliveryRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
