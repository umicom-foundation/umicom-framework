/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/events.h
 *
 * PURPOSE:
 *   Publish immutable layout lifecycle facts to interested Framework controllers without sharing mutable layout pointers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_EVENTS_H
#define UMICOM_WORKBENCH_LAYOUT_EVENTS_H

#include "umicom/workbench_layout/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout event data shared with callers of this public contract.
 */
typedef struct UmiWorkbenchLayoutEvent {
    uint32_t structure_size;
    UmiWorkbenchLayoutEventKind kind;
    char event_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char layout_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char session_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char actor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char causation_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char message[UMI_WORKBENCH_LAYOUT_DESCRIPTION_CAPACITY];
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t layout_revision;
    UmiStatus status;
} UmiWorkbenchLayoutEvent;

typedef void (*UmiWorkbenchLayoutEventListener)(
    void *context,
    const UmiWorkbenchLayoutEvent *event);

/**
 * Represent the workbench layout event subscription data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchLayoutEventSubscription {
    uint32_t structure_size;
    char subscription_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t event_mask;
    UmiWorkbenchLayoutEventListener listener;
    void *listener_context;
    bool active;
} UmiWorkbenchLayoutEventSubscription;

/**
 * Represent the workbench layout event bus data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutEventBus {
    uint32_t structure_size;
    UmiWorkbenchLayoutEventSubscription
        subscriptions[UMI_WORKBENCH_LAYOUT_MAX_LISTENERS];
    size_t subscription_count;
    uint64_t next_sequence;
    uint64_t published_count;
    uint64_t revision;
} UmiWorkbenchLayoutEventBus;

/**
 * Initialise workbench layout event bus from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_event_bus_init(
    UmiWorkbenchLayoutEventBus *bus);

/**
 * Initialise workbench layout event from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_event_init(
    UmiWorkbenchLayoutEvent *event,
    UmiWorkbenchLayoutEventKind kind,
    const char *event_id,
    const char *layout_id);

/**
 * Provide the workbench layout event bus subscribe operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_event_bus_subscribe(
    UmiWorkbenchLayoutEventBus *bus,
    const UmiWorkbenchLayoutEventSubscription *subscription);

/**
 * Provide the workbench layout event bus unsubscribe operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_event_bus_unsubscribe(
    UmiWorkbenchLayoutEventBus *bus,
    const char *subscription_id);

/**
 * Provide the workbench layout event bus publish operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_event_bus_publish(
    UmiWorkbenchLayoutEventBus *bus,
    UmiWorkbenchLayoutEvent *event);

/**
 * Provide the workbench layout event mask operation used by this module and its client
 * applications.
 */
uint64_t umi_workbench_layout_event_mask(
    UmiWorkbenchLayoutEventKind kind);

/**
 * Provide the workbench layout event subscription accepts operation used by this module
 * and its client applications.
 */
bool umi_workbench_layout_event_subscription_accepts(
    const UmiWorkbenchLayoutEventSubscription *subscription,
    UmiWorkbenchLayoutEventKind kind);

#ifdef __cplusplus
}
#endif

#endif
