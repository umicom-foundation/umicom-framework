/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/rate_sample.h
 *
 * PURPOSE:
 *   Define the reusable event rate sample contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_RATE_SAMPLE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_RATE_SAMPLE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event rate sample data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventRateSample {
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
} UmiWorkbenchContextEventRateSample;

/**
 * Initialise workbench context event rate sample from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_rate_sample_init(
    UmiWorkbenchContextEventRateSample *record,
    const char *record_id);
/**
 * Check that workbench context event rate sample satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_rate_sample_validate(
    const UmiWorkbenchContextEventRateSample *record);
/**
 * Provide the workbench context event rate sample set source operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_rate_sample_set_source(
    UmiWorkbenchContextEventRateSample *record,
    const char *source_id);
/**
 * Provide the workbench context event rate sample set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_rate_sample_set_subject(
    UmiWorkbenchContextEventRateSample *record,
    const char *subject_id);
/**
 * Provide the workbench context event rate sample set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_rate_sample_set_group(
    UmiWorkbenchContextEventRateSample *record,
    const char *group_id);
/**
 * Provide the workbench context event rate sample set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_rate_sample_set_label(
    UmiWorkbenchContextEventRateSample *record,
    const char *label);
/**
 * Provide the workbench context event rate sample hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_event_rate_sample_hash(
    const UmiWorkbenchContextEventRateSample *record);
/**
 * Provide the workbench context event rate sample touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_event_rate_sample_touch(
    UmiWorkbenchContextEventRateSample *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
