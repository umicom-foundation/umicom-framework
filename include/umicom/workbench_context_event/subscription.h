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

/**
 * Represent the workbench context event subscription data shared with callers of this
 * public contract.
 */
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

/**
 * Initialise workbench context event subscription from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_event_subscription_init(
    UmiWorkbenchContextEventSubscription *record,
    const char *record_id);
/**
 * Check that workbench context event subscription satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_event_subscription_validate(
    const UmiWorkbenchContextEventSubscription *record);
/**
 * Provide the workbench context event subscription set source operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_subscription_set_source(
    UmiWorkbenchContextEventSubscription *record,
    const char *source_id);
/**
 * Provide the workbench context event subscription set subject operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_event_subscription_set_subject(
    UmiWorkbenchContextEventSubscription *record,
    const char *subject_id);
/**
 * Provide the workbench context event subscription set group operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_subscription_set_group(
    UmiWorkbenchContextEventSubscription *record,
    const char *group_id);
/**
 * Provide the workbench context event subscription set label operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_event_subscription_set_label(
    UmiWorkbenchContextEventSubscription *record,
    const char *label);
/**
 * Provide the workbench context event subscription hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_event_subscription_hash(
    const UmiWorkbenchContextEventSubscription *record);
/**
 * Provide the workbench context event subscription touch operation used by this module and
 * its client applications.
 */
void umi_workbench_context_event_subscription_touch(
    UmiWorkbenchContextEventSubscription *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

#ifdef __cplusplus
}
#endif
#endif
