/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/protocol/transport.h
 *
 * PURPOSE:
 *   Define a deployment-neutral protocol transport and an in-memory deterministic adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROTOCOL_TRANSPORT_H
#define UMICOM_PROTOCOL_TRANSPORT_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/protocol/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the protocol transport data shared with callers of this public contract.
 */
typedef struct UmiProtocolTransport UmiProtocolTransport;

/**
 * Represent the protocol transport stats data shared with callers of this public contract.
 */
typedef struct UmiProtocolTransportStats {
    size_t queued;
    size_t sent;
    size_t received;
    size_t dropped;
} UmiProtocolTransportStats;

/**
 * Provide the protocol transport create memory operation used by this module and its
 * client applications.
 */
UmiStatus umi_protocol_transport_create_memory(
    size_t capacity,
    UmiProtocolTransport **out_transport
);
/**
 * Release or reset state held by protocol transport so the same storage can be reused
 * safely.
 */
void umi_protocol_transport_destroy(UmiProtocolTransport *transport);
/**
 * Provide the protocol transport send operation used by this module and its client
 * applications.
 */
UmiStatus umi_protocol_transport_send(UmiProtocolTransport *transport,
                                      const char *message);
/**
 * Provide the protocol transport receive operation used by this module and its client
 * applications.
 */
UmiStatus umi_protocol_transport_receive(UmiProtocolTransport *transport,
                                         char *out_message,
                                         size_t capacity);
/**
 * Return the number of records represented by protocol transport without changing their
 * state.
 */
size_t umi_protocol_transport_count(const UmiProtocolTransport *transport);
/**
 * Provide the protocol transport stats operation used by this module and its client
 * applications.
 */
UmiProtocolTransportStats umi_protocol_transport_stats(
    const UmiProtocolTransport *transport
);

#ifdef __cplusplus
}
#endif

#endif
