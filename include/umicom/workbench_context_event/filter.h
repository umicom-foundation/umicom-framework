/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/filter.h
 *
 * PURPOSE:
 *   Define the reusable event filter contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_FILTER_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_FILTER_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event filter data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventFilter {
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
} UmiWorkbenchContextEventFilter;

/**
 * Initialise workbench context event filter from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_filter_init(
    UmiWorkbenchContextEventFilter *record,
    const char *record_id);
/**
 * Check that workbench context event filter satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_event_filter_validate(
    const UmiWorkbenchContextEventFilter *record);
/**
 * Provide the workbench context event filter set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_filter_set_source(
    UmiWorkbenchContextEventFilter *record,
    const char *source_id);
/**
 * Provide the workbench context event filter set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_filter_set_subject(
    UmiWorkbenchContextEventFilter *record,
    const char *subject_id);
/**
 * Provide the workbench context event filter set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_filter_set_group(
    UmiWorkbenchContextEventFilter *record,
    const char *group_id);
/**
 * Provide the workbench context event filter set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_filter_set_label(
    UmiWorkbenchContextEventFilter *record,
    const char *label);
/**
 * Provide the workbench context event filter hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_filter_hash(
    const UmiWorkbenchContextEventFilter *record);
/**
 * Provide the workbench context event filter touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_event_filter_touch(
    UmiWorkbenchContextEventFilter *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
