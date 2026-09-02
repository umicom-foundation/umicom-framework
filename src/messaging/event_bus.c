/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/event_bus.c
 *
 * PURPOSE:
 *   Implement the compatibility event bus on the canonical message envelope while preserving deterministic sequence and correlation identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/event_bus.h"

#include <stdio.h>
#include <stdlib.h>
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_bus == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bus = (UmiEventBus *)calloc(1U, sizeof(UmiEventBus));
    return *out_bus != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/* Release or reset state held by event bus so the same storage can be reused safely. */
void umi_event_bus_destroy(UmiEventBus *bus)
{
    free(bus);
}

/*
 * Provide the event bus subscribe operation used by this module and its client
 * applications.
 */
UmiStatus umi_event_bus_subscribe(UmiEventBus *bus,
                                  const char *topic,
                                  UmiEventHandler handler,
                                  void *user_data)
{
    UmiEventSubscription *subscription;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bus == NULL || topic == NULL || topic[0] == '\0' || handler == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (bus->count >= UMI_EVENT_MAX_SUBSCRIPTIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    subscription = &bus->subscriptions[bus->count++];
    (void)snprintf(subscription->topic,
                   sizeof(subscription->topic),
                   "%s",
                   topic);
    subscription->handler = handler;
    subscription->user_data = user_data;
    return UMI_STATUS_OK;
}

/* Provide the event bus publish operation used by this module and its client applications. */
UmiStatus umi_event_bus_publish(UmiEventBus *bus,
                                const char *topic,
                                const char *payload,
                                uint64_t correlation_id)
{
    UmiMessageEnvelope event;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bus == NULL || topic == NULL || topic[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    bus->sequence++;
    umi_message_envelope_init(&event,
                              UMI_MESSAGE_EVENT,
                              topic,
                              payload != NULL ? payload : "");
    event.sequence = bus->sequence;
    event.correlation_id = correlation_id;
    event.schema_id = topic;
    event.source = "umicom.event-bus";
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < bus->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(bus->subscriptions[index].topic, topic) == 0) {
            bus->subscriptions[index].handler(
                &event,
                bus->subscriptions[index].user_data);
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
    return bus != NULL ? bus->sequence : 0U;
}
