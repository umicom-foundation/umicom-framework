/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/change_broker.h
 *
 * PURPOSE:
 *   Combine the existing in-process event bus and append-only journal into a
 *   small durable publish/subscribe service for Framework change events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_CHANGE_BROKER_H
#define UMICOM_MESSAGING_CHANGE_BROKER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/event_bus.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Keep the implementation private so applications use the same safe
 * operations regardless of how events are stored or delivered.
 */
typedef struct UmiChangeBroker UmiChangeBroker;

/**
 * Configure a broker.  An empty journal path creates a memory-only broker;
 * a non-empty path records every accepted event before subscribers see it.
 */
typedef struct UmiChangeBrokerConfig {
    uint32_t structure_size;
    const char *journal_path;
    const char *source_id;
} UmiChangeBrokerConfig;

/** Return conservative defaults suitable for an embedded application. */
UmiChangeBrokerConfig umi_change_broker_config_default(void);

/** Create an independent broker and, when requested, open its event journal. */
UmiStatus umi_change_broker_create(const UmiChangeBrokerConfig *config,
                                   UmiChangeBroker **out_broker);

/** Close the journal, release the event bus and discard broker-owned memory. */
void umi_change_broker_destroy(UmiChangeBroker *broker);

/** Register a callback for one exact topic without transferring its ownership. */
UmiStatus umi_change_broker_subscribe(UmiChangeBroker *broker,
                                      const char *topic,
                                      UmiEventHandler handler,
                                      void *user_data);

/**
 * Record and publish a UTF-8 change event.  Durable brokers write the journal
 * first, so a storage failure cannot be mistaken for a recorded change.
 */
UmiStatus umi_change_broker_publish(UmiChangeBroker *broker,
                                    const char *topic,
                                    const char *payload,
                                    uint64_t correlation_id);

/** Replay the durable journal through the current subscriptions. */
UmiStatus umi_change_broker_replay(UmiChangeBroker *broker,
                                   size_t *out_event_count);

/** Report the number assigned to the most recently accepted change. */
uint64_t umi_change_broker_last_sequence(const UmiChangeBroker *broker);

/** Return non-zero when this broker records events in an append-only journal. */
int umi_change_broker_is_durable(const UmiChangeBroker *broker);

#ifdef __cplusplus
}
#endif

#endif
