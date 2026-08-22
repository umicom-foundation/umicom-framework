/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/coalescing_policy.h
 *
 * PURPOSE:
 *   Define the reusable event coalescing policy contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_COALESCING_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_COALESCING_POLICY_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_workbench_context_event_coalescing_policy_init(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_coalescing_policy_validate(
    const UmiWorkbenchContextEventCoalescingPolicy *record);
UmiStatus umi_workbench_context_event_coalescing_policy_set_source(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_coalescing_policy_set_subject(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_coalescing_policy_set_group(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_coalescing_policy_set_label(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    const char *label);
uint64_t umi_workbench_context_event_coalescing_policy_hash(
    const UmiWorkbenchContextEventCoalescingPolicy *record);
void umi_workbench_context_event_coalescing_policy_touch(
    UmiWorkbenchContextEventCoalescingPolicy *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
