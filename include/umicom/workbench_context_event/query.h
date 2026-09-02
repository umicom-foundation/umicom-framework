/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/query.h
 *
 * PURPOSE:
 *   Define the reusable event query contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_QUERY_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_QUERY_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event query data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventQuery {
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
} UmiWorkbenchContextEventQuery;

/**
 * Initialise workbench context event query from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_event_query_init(
    UmiWorkbenchContextEventQuery *record,
    const char *record_id);
/**
 * Check that workbench context event query satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_event_query_validate(
    const UmiWorkbenchContextEventQuery *record);
/**
 * Provide the workbench context event query set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_query_set_source(
    UmiWorkbenchContextEventQuery *record,
    const char *source_id);
/**
 * Provide the workbench context event query set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_query_set_subject(
    UmiWorkbenchContextEventQuery *record,
    const char *subject_id);
/**
 * Provide the workbench context event query set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_query_set_group(
    UmiWorkbenchContextEventQuery *record,
    const char *group_id);
/**
 * Provide the workbench context event query set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_query_set_label(
    UmiWorkbenchContextEventQuery *record,
    const char *label);
/**
 * Provide the workbench context event query hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_query_hash(
    const UmiWorkbenchContextEventQuery *record);
/**
 * Provide the workbench context event query touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_event_query_touch(
    UmiWorkbenchContextEventQuery *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
