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

UmiStatus umi_event_bus_create(UmiEventBus **out_bus)
{
    if (out_bus == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_bus = calloc(1U, sizeof(UmiEventBus));
    return *out_bus != 0 ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_event_bus_destroy(UmiEventBus *bus) { free(bus); }

UmiStatus umi_event_bus_subscribe(UmiEventBus *bus, const char *topic,
                                  UmiEventHandler handler, void *user_data)
{
    UmiEventSubscription *subscription;
    if (bus == 0 || topic == 0 || topic[0] == '\0' || handler == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (bus->count >= UMI_EVENT_MAX_SUBSCRIPTIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    subscription = &bus->subscriptions[bus->count++];
    (void)snprintf(subscription->topic, sizeof(subscription->topic), "%s", topic);
    subscription->handler = handler;
    subscription->user_data = user_data;
    return UMI_STATUS_OK;
}

UmiStatus umi_event_bus_publish(UmiEventBus *bus, const char *topic,
                                const char *payload, uint64_t correlation_id)
{
    UmiMessageEnvelope event;
    size_t index;
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
    for (index = 0U; index < bus->count; ++index) {
        if (strcmp(bus->subscriptions[index].topic, topic) == 0) {
            bus->subscriptions[index].handler(&event, bus->subscriptions[index].user_data);
        }
    }
    return UMI_STATUS_OK;
}

uint64_t umi_event_bus_last_sequence(const UmiEventBus *bus)
{
    return bus != 0 ? bus->sequence : 0U;
}
