/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/events.h
 *
 * PURPOSE:
 *   Publish immutable layout lifecycle facts to interested Framework controllers without sharing mutable layout pointers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_EVENTS_H
#define UMICOM_WORKBENCH_LAYOUT_EVENTS_H

#include "umicom/workbench_layout/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct UmiWorkbenchLayoutEventSubscription {
    uint32_t structure_size;
    char subscription_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    uint64_t event_mask;
    UmiWorkbenchLayoutEventListener listener;
    void *listener_context;
    bool active;
} UmiWorkbenchLayoutEventSubscription;

typedef struct UmiWorkbenchLayoutEventBus {
    uint32_t structure_size;
    UmiWorkbenchLayoutEventSubscription
        subscriptions[UMI_WORKBENCH_LAYOUT_MAX_LISTENERS];
    size_t subscription_count;
    uint64_t next_sequence;
    uint64_t published_count;
    uint64_t revision;
} UmiWorkbenchLayoutEventBus;

void umi_workbench_layout_event_bus_init(
    UmiWorkbenchLayoutEventBus *bus);

void umi_workbench_layout_event_init(
    UmiWorkbenchLayoutEvent *event,
    UmiWorkbenchLayoutEventKind kind,
    const char *event_id,
    const char *layout_id);

UmiStatus umi_workbench_layout_event_bus_subscribe(
    UmiWorkbenchLayoutEventBus *bus,
    const UmiWorkbenchLayoutEventSubscription *subscription);

UmiStatus umi_workbench_layout_event_bus_unsubscribe(
    UmiWorkbenchLayoutEventBus *bus,
    const char *subscription_id);

UmiStatus umi_workbench_layout_event_bus_publish(
    UmiWorkbenchLayoutEventBus *bus,
    UmiWorkbenchLayoutEvent *event);

uint64_t umi_workbench_layout_event_mask(
    UmiWorkbenchLayoutEventKind kind);

bool umi_workbench_layout_event_subscription_accepts(
    const UmiWorkbenchLayoutEventSubscription *subscription,
    UmiWorkbenchLayoutEventKind kind);

#ifdef __cplusplus
}
#endif

#endif
