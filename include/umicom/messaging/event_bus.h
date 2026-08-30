/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/event_bus.h
 *
 * PURPOSE:
 *   Publish the public event bus contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_EVENT_BUS_H
#define UMICOM_MESSAGING_EVENT_BUS_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEventBus UmiEventBus;
typedef void (*UmiEventHandler)(const UmiMessageEnvelope *event, void *user_data);

UmiStatus umi_event_bus_create(UmiEventBus **out_bus);
void umi_event_bus_destroy(UmiEventBus *bus);
UmiStatus umi_event_bus_subscribe(UmiEventBus *bus, const char *topic,
                                  UmiEventHandler handler, void *user_data);
UmiStatus umi_event_bus_publish(UmiEventBus *bus, const char *topic,
                                const char *payload, uint64_t correlation_id);
uint64_t umi_event_bus_last_sequence(const UmiEventBus *bus);

#ifdef __cplusplus
}
#endif

#endif
