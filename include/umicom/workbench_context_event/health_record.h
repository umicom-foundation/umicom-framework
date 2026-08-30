/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/health_record.h
 *
 * PURPOSE:
 *   Define the reusable event health record contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_HEALTH_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_HEALTH_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventHealthRecord {
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
} UmiWorkbenchContextEventHealthRecord;

void umi_workbench_context_event_health_record_init(
    UmiWorkbenchContextEventHealthRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_health_record_validate(
    const UmiWorkbenchContextEventHealthRecord *record);
UmiStatus umi_workbench_context_event_health_record_set_source(
    UmiWorkbenchContextEventHealthRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_health_record_set_subject(
    UmiWorkbenchContextEventHealthRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_health_record_set_group(
    UmiWorkbenchContextEventHealthRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_health_record_set_label(
    UmiWorkbenchContextEventHealthRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_health_record_hash(
    const UmiWorkbenchContextEventHealthRecord *record);
void umi_workbench_context_event_health_record_touch(
    UmiWorkbenchContextEventHealthRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
