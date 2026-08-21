/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/events.c
 *
 * PURPOSE:
 *   Implement immutable layout lifecycle events and bounded subscription dispatch without exposing mutable document pointers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/events.h"

#include <string.h>

#include "internal.h"

void umi_workbench_layout_event_bus_init(
    UmiWorkbenchLayoutEventBus *bus)
{
    if (bus == NULL) {
        return;
    }
    (void)memset(bus, 0, sizeof(*bus));
    bus->structure_size = sizeof(*bus);
    bus->next_sequence = 1U;
    bus->revision = 1U;
}

void umi_workbench_layout_event_init(
    UmiWorkbenchLayoutEvent *event,
    UmiWorkbenchLayoutEventKind kind,
    const char *event_id,
    const char *layout_id)
{
    if (event == NULL) {
        return;
    }

    (void)memset(event, 0, sizeof(*event));
    event->structure_size = sizeof(*event);
    event->kind = kind;
    event->status = UMI_STATUS_OK;
    if (event_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            event->event_id,
            sizeof(event->event_id),
            event_id,
            true);
    }
    if (layout_id != NULL) {
        (void)umi_workbench_layout_copy_text(
            event->layout_id,
            sizeof(event->layout_id),
            layout_id,
            true);
    }
}

uint64_t umi_workbench_layout_event_mask(
    UmiWorkbenchLayoutEventKind kind)
{
    if (kind < UMI_WORKBENCH_LAYOUT_EVENT_CREATED ||
        kind > UMI_WORKBENCH_LAYOUT_EVENT_CONFLICT_DETECTED) {
        return 0U;
    }
    return UINT64_C(1) << ((uint64_t)kind - 1U);
}

bool umi_workbench_layout_event_subscription_accepts(
    const UmiWorkbenchLayoutEventSubscription *subscription,
    UmiWorkbenchLayoutEventKind kind)
{
    uint64_t mask = umi_workbench_layout_event_mask(kind);
    return subscription != NULL &&
           subscription->active &&
           subscription->listener != NULL &&
           mask != 0U &&
           (subscription->event_mask & mask) != 0U;
}

UmiStatus umi_workbench_layout_event_bus_subscribe(
    UmiWorkbenchLayoutEventBus *bus,
    const UmiWorkbenchLayoutEventSubscription *subscription)
{
    size_t index;

    if (bus == NULL || subscription == NULL ||
        subscription->structure_size < sizeof(*subscription) ||
        !umi_workbench_layout_text_present(
            subscription->subscription_id) ||
        subscription->listener == NULL ||
        subscription->event_mask == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (bus->subscription_count >=
        UMI_WORKBENCH_LAYOUT_MAX_LISTENERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < bus->subscription_count; ++index) {
        if (strcmp(
                bus->subscriptions[index].subscription_id,
                subscription->subscription_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    bus->subscriptions[bus->subscription_count] =
        *subscription;
    bus->subscriptions[bus->subscription_count].structure_size =
        sizeof(bus->subscriptions[bus->subscription_count]);
    bus->subscription_count += 1U;
    bus->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_event_bus_unsubscribe(
    UmiWorkbenchLayoutEventBus *bus,
    const char *subscription_id)
{
    size_t index;

    if (bus == NULL ||
        !umi_workbench_layout_text_present(subscription_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U;
         index < bus->subscription_count;
         ++index) {
        if (strcmp(
                bus->subscriptions[index].subscription_id,
                subscription_id) != 0) {
            continue;
        }

        for (; index + 1U < bus->subscription_count; ++index) {
            bus->subscriptions[index] =
                bus->subscriptions[index + 1U];
        }
        bus->subscription_count -= 1U;
        (void)memset(
            &bus->subscriptions[bus->subscription_count],
            0,
            sizeof(bus->subscriptions[bus->subscription_count]));
        bus->revision += 1U;
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_workbench_layout_event_bus_publish(
    UmiWorkbenchLayoutEventBus *bus,
    UmiWorkbenchLayoutEvent *event)
{
    UmiWorkbenchLayoutEventSubscription
        subscriptions[UMI_WORKBENCH_LAYOUT_MAX_LISTENERS];
    size_t subscription_count;
    size_t index;

    if (bus == NULL || event == NULL ||
        event->structure_size < sizeof(*event) ||
        event->kind < UMI_WORKBENCH_LAYOUT_EVENT_CREATED ||
        event->kind >
            UMI_WORKBENCH_LAYOUT_EVENT_CONFLICT_DETECTED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    event->sequence = bus->next_sequence;
    bus->next_sequence += 1U;
    bus->published_count += 1U;
    bus->revision += 1U;

    subscription_count = bus->subscription_count;
    (void)memcpy(
        subscriptions,
        bus->subscriptions,
        sizeof(subscriptions));

    for (index = 0U;
         index < subscription_count;
         ++index) {
        if (umi_workbench_layout_event_subscription_accepts(
                &subscriptions[index], event->kind)) {
            subscriptions[index].listener(
                subscriptions[index].listener_context,
                event);
        }
    }
    return UMI_STATUS_OK;
}
