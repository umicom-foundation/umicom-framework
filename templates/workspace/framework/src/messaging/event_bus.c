/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/messaging/event_bus.c
 *
 * PURPOSE:
 *   Implement the event bus behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/event_bus.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define UMI_EVENT_MAX_SUBSCRIPTIONS 128U

typedef struct UmiEventSubscription {
    char topic[96];
    UmiEventHandler handler;
    void *user_data;
} UmiEventSubscription;

struct UmiEventBus {
    UmiEventSubscription subscriptions[UMI_EVENT_MAX_SUBSCRIPTIONS];
    size_t count;
    uint64_t sequence;
};

/*
 * Initialise event bus from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_event_bus_create(UmiEventBus **out_bus)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_bus == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bus = calloc(1U, sizeof(UmiEventBus));
    return *out_bus != 0 ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/* Release or reset state held by event bus so the same storage can be reused safely. */
void umi_event_bus_destroy(UmiEventBus *bus) { free(bus); }

/*
 * Provide the event bus subscribe operation used by this module and its client
 * applications.
 */
UmiStatus umi_event_bus_subscribe(UmiEventBus *bus, const char *topic,
                                  UmiEventHandler handler, void *user_data)
{
    UmiEventSubscription *subscription;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bus == 0 || topic == 0 || topic[0] == '\0' || handler == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (bus->count >= UMI_EVENT_MAX_SUBSCRIPTIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    subscription = &bus->subscriptions[bus->count++];
    (void)snprintf(subscription->topic, sizeof(subscription->topic), "%s", topic);
    subscription->handler = handler;
    subscription->user_data = user_data;
    return UMI_STATUS_OK;
}

/* Provide the event bus publish operation used by this module and its client applications. */
UmiStatus umi_event_bus_publish(UmiEventBus *bus, const char *topic,
                                const char *payload, uint64_t correlation_id)
{
    UmiMessageEnvelope event;
    size_t index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (bus == 0 || topic == 0 || topic[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    bus->sequence++;
    event.structure_size = (uint32_t)sizeof(event);
    event.schema_version = 1U;
    event.kind = UMI_MESSAGE_EVENT;
    event.sequence = bus->sequence;
    event.correlation_id = correlation_id;
    event.name = topic;
    event.payload = payload != 0 ? payload : "";
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bus->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(bus->subscriptions[index].topic, topic) == 0) {
            bus->subscriptions[index].handler(&event, bus->subscriptions[index].user_data);
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the event bus last sequence operation used by this module and its client
 * applications.
 */
uint64_t umi_event_bus_last_sequence(const UmiEventBus *bus)
{
    return bus != 0 ? bus->sequence : 0U;
}
