/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/policy.h
 *
 * PURPOSE:
 *   Define the reusable event policy contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_POLICY_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event policy data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextEventPolicy {
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
} UmiWorkbenchContextEventPolicy;

/**
 * Initialise workbench context event policy from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_policy_init(
    UmiWorkbenchContextEventPolicy *record,
    const char *record_id);
/**
 * Check that workbench context event policy satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_context_event_policy_validate(
    const UmiWorkbenchContextEventPolicy *record);
/**
 * Provide the workbench context event policy set source operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_policy_set_source(
    UmiWorkbenchContextEventPolicy *record,
    const char *source_id);
/**
 * Provide the workbench context event policy set subject operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_policy_set_subject(
    UmiWorkbenchContextEventPolicy *record,
    const char *subject_id);
/**
 * Provide the workbench context event policy set group operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_policy_set_group(
    UmiWorkbenchContextEventPolicy *record,
    const char *group_id);
/**
 * Provide the workbench context event policy set label operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_event_policy_set_label(
    UmiWorkbenchContextEventPolicy *record,
    const char *label);
/**
 * Provide the workbench context event policy hash operation used by this module and its
 * client applications.
 */
uint64_t umi_workbench_context_event_policy_hash(
    const UmiWorkbenchContextEventPolicy *record);
/**
 * Provide the workbench context event policy touch operation used by this module and its
 * client applications.
 */
void umi_workbench_context_event_policy_touch(
    UmiWorkbenchContextEventPolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
