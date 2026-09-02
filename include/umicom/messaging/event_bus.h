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

/**
 * Represent the event bus data shared with callers of this public contract.
 */
typedef struct UmiEventBus UmiEventBus;
typedef void (*UmiEventHandler)(const UmiMessageEnvelope *event, void *user_data);

/**
 * Initialise event bus from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_event_bus_create(UmiEventBus **out_bus);
/**
 * Release or reset state held by event bus so the same storage can be reused safely.
 */
void umi_event_bus_destroy(UmiEventBus *bus);
/**
 * Provide the event bus subscribe operation used by this module and its client
 * applications.
 */
UmiStatus umi_event_bus_subscribe(UmiEventBus *bus, const char *topic,
                                  UmiEventHandler handler, void *user_data);
/**
 * Provide the event bus publish operation used by this module and its client applications.
 */
UmiStatus umi_event_bus_publish(UmiEventBus *bus, const char *topic,
                                const char *payload, uint64_t correlation_id);
/**
 * Provide the event bus last sequence operation used by this module and its client
 * applications.
 */
uint64_t umi_event_bus_last_sequence(const UmiEventBus *bus);

#ifdef __cplusplus
}
#endif

#endif
