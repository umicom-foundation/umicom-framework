/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/correlation_record.h
 *
 * PURPOSE:
 *   Define the reusable event correlation record contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_CORRELATION_RECORD_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_CORRELATION_RECORD_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventCorrelationRecord {
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
} UmiWorkbenchContextEventCorrelationRecord;

void umi_workbench_context_event_correlation_record_init(
    UmiWorkbenchContextEventCorrelationRecord *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_correlation_record_validate(
    const UmiWorkbenchContextEventCorrelationRecord *record);
UmiStatus umi_workbench_context_event_correlation_record_set_source(
    UmiWorkbenchContextEventCorrelationRecord *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_correlation_record_set_subject(
    UmiWorkbenchContextEventCorrelationRecord *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_correlation_record_set_group(
    UmiWorkbenchContextEventCorrelationRecord *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_correlation_record_set_label(
    UmiWorkbenchContextEventCorrelationRecord *record,
    const char *label);
uint64_t umi_workbench_context_event_correlation_record_hash(
    const UmiWorkbenchContextEventCorrelationRecord *record);
void umi_workbench_context_event_correlation_record_touch(
    UmiWorkbenchContextEventCorrelationRecord *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
