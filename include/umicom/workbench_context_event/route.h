/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/route.h
 *
 * PURPOSE:
 *   Define the reusable event routing rule contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_ROUTE_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_ROUTE_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event route data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventRoute {
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
} UmiWorkbenchContextEventRoute;

/**
 * Initialise workbench context event route from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_context_event_route_init(
    UmiWorkbenchContextEventRoute *record,
    const char *record_id);
/**
 * Check that workbench context event route satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_event_route_validate(
    const UmiWorkbenchContextEventRoute *record);
/**
 * Provide the workbench context event route set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_route_set_source(
    UmiWorkbenchContextEventRoute *record,
    const char *source_id);
/**
 * Provide the workbench context event route set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_route_set_subject(
    UmiWorkbenchContextEventRoute *record,
    const char *subject_id);
/**
 * Provide the workbench context event route set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_route_set_group(
    UmiWorkbenchContextEventRoute *record,
    const char *group_id);
/**
 * Provide the workbench context event route set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_route_set_label(
    UmiWorkbenchContextEventRoute *record,
    const char *label);
/**
 * Provide the workbench context event route hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_route_hash(
    const UmiWorkbenchContextEventRoute *record);
/**
 * Provide the workbench context event route touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_event_route_touch(
    UmiWorkbenchContextEventRoute *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
