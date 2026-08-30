/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/latency_sample.h
 *
 * PURPOSE:
 *   Define the reusable event latency sample contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_LATENCY_SAMPLE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_LATENCY_SAMPLE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventLatencySample {
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
} UmiWorkbenchContextEventLatencySample;

void umi_workbench_context_event_latency_sample_init(
    UmiWorkbenchContextEventLatencySample *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_latency_sample_validate(
    const UmiWorkbenchContextEventLatencySample *record);
UmiStatus umi_workbench_context_event_latency_sample_set_source(
    UmiWorkbenchContextEventLatencySample *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_latency_sample_set_subject(
    UmiWorkbenchContextEventLatencySample *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_latency_sample_set_group(
    UmiWorkbenchContextEventLatencySample *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_latency_sample_set_label(
    UmiWorkbenchContextEventLatencySample *record,
    const char *label);
uint64_t umi_workbench_context_event_latency_sample_hash(
    const UmiWorkbenchContextEventLatencySample *record);
void umi_workbench_context_event_latency_sample_touch(
    UmiWorkbenchContextEventLatencySample *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
