/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_event/subscription.h
 *
 * PURPOSE:
 *   Define the reusable event subscription contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_EVENT_SUBSCRIPTION_H
#define UMICOM_WORKBENCH_CONTEXT_EVENT_SUBSCRIPTION_H

#include "umicom/workbench_context_event/event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextEventSubscription {
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
} UmiWorkbenchContextEventSubscription;

void umi_workbench_context_event_subscription_init(
    UmiWorkbenchContextEventSubscription *record,
    const char *record_id);
UmiStatus umi_workbench_context_event_subscription_validate(
    const UmiWorkbenchContextEventSubscription *record);
UmiStatus umi_workbench_context_event_subscription_set_source(
    UmiWorkbenchContextEventSubscription *record,
    const char *source_id);
UmiStatus umi_workbench_context_event_subscription_set_subject(
    UmiWorkbenchContextEventSubscription *record,
    const char *subject_id);
UmiStatus umi_workbench_context_event_subscription_set_group(
    UmiWorkbenchContextEventSubscription *record,
    const char *group_id);
UmiStatus umi_workbench_context_event_subscription_set_label(
    UmiWorkbenchContextEventSubscription *record,
    const char *label);
uint64_t umi_workbench_context_event_subscription_hash(
    const UmiWorkbenchContextEventSubscription *record);
void umi_workbench_context_event_subscription_touch(
    UmiWorkbenchContextEventSubscription *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
