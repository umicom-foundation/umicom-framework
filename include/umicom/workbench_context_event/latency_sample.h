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

/**
 * Represent the workbench context event latency sample data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context event latency sample from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_latency_sample_init(
    UmiWorkbenchContextEventLatencySample *record,
    const char *record_id);
/**
 * Check that workbench context event latency sample satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_latency_sample_validate(
    const UmiWorkbenchContextEventLatencySample *record);
/**
 * Provide the workbench context event latency sample set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_latency_sample_set_source(
    UmiWorkbenchContextEventLatencySample *record,
    const char *source_id);
/**
 * Provide the workbench context event latency sample set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_latency_sample_set_subject(
    UmiWorkbenchContextEventLatencySample *record,
    const char *subject_id);
/**
 * Provide the workbench context event latency sample set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_latency_sample_set_group(
    UmiWorkbenchContextEventLatencySample *record,
    const char *group_id);
/**
 * Provide the workbench context event latency sample set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_latency_sample_set_label(
    UmiWorkbenchContextEventLatencySample *record,
    const char *label);
/**
 * Provide the workbench context event latency sample hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_latency_sample_hash(
    const UmiWorkbenchContextEventLatencySample *record);
/**
 * Provide the workbench context event latency sample touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_event_latency_sample_touch(
    UmiWorkbenchContextEventLatencySample *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
