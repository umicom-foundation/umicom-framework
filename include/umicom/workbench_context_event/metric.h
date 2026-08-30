/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/metric.h
 *
 * PURPOSE:
 *   Define the reusable event metric contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_METRIC_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_METRIC_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventMetric {
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
} UmiWorkbenchContextEventMetric;

void umi_workbench_context_event_metric_init(
    UmiWorkbenchContextEventMetric *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_metric_validate(
    const UmiWorkbenchContextEventMetric *record);
UmiStatus umi_workbench_context_event_metric_set_source(
    UmiWorkbenchContextEventMetric *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_metric_set_subject(
    UmiWorkbenchContextEventMetric *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_metric_set_group(
    UmiWorkbenchContextEventMetric *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_metric_set_label(
    UmiWorkbenchContextEventMetric *record,
    const char *label);
uint64_t umi_workbench_context_event_metric_hash(
    const UmiWorkbenchContextEventMetric *record);
void umi_workbench_context_event_metric_touch(
    UmiWorkbenchContextEventMetric *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
