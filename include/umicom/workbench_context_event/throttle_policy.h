/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/throttle_policy.h
 *
 * PURPOSE:
 *   Define the reusable event throttle policy contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_THROTTLE_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_THROTTLE_POLICY_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventThrottlePolicy {
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
} UmiWorkbenchContextEventThrottlePolicy;

void umi_workbench_context_event_throttle_policy_init(
    UmiWorkbenchContextEventThrottlePolicy *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_throttle_policy_validate(
    const UmiWorkbenchContextEventThrottlePolicy *record);
UmiStatus umi_workbench_context_event_throttle_policy_set_source(
    UmiWorkbenchContextEventThrottlePolicy *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_throttle_policy_set_subject(
    UmiWorkbenchContextEventThrottlePolicy *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_throttle_policy_set_group(
    UmiWorkbenchContextEventThrottlePolicy *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_throttle_policy_set_label(
    UmiWorkbenchContextEventThrottlePolicy *record,
    const char *label);
uint64_t umi_workbench_context_event_throttle_policy_hash(
    const UmiWorkbenchContextEventThrottlePolicy *record);
void umi_workbench_context_event_throttle_policy_touch(
    UmiWorkbenchContextEventThrottlePolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
