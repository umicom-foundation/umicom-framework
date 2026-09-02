/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/coalescing_policy.h
 *
 * PURPOSE:
 *   Define the reusable event coalescing policy contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_COALESCING_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_COALESCING_POLICY_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context event coalescing policy data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextEventCoalescingPolicy {
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
} UmiWorkbenchContextEventCoalescingPolicy;

/**
 * Initialise workbench context event coalescing policy from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_event_coalescing_policy_init(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    const char *record_id);
/**
 * Check that workbench context event coalescing policy satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_event_coalescing_policy_validate(
    const UmiWorkbenchContextEventCoalescingPolicy *record);
/**
 * Provide the workbench context event coalescing policy set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_coalescing_policy_set_source(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    const char *source_id);
/**
 * Provide the workbench context event coalescing policy set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_coalescing_policy_set_subject(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    const char *subject_id);
/**
 * Provide the workbench context event coalescing policy set group operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_coalescing_policy_set_group(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    const char *group_id);
/**
 * Provide the workbench context event coalescing policy set label operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_coalescing_policy_set_label(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    const char *label);
/**
 * Provide the workbench context event coalescing policy hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_event_coalescing_policy_hash(
    const UmiWorkbenchContextEventCoalescingPolicy *record);
/**
 * Provide the workbench context event coalescing policy touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_event_coalescing_policy_touch(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
