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

typedef struct UmiProtocolTransport UmiProtocolTransport;

typedef struct UmiProtocolTransportStats {
    size_t queued;
    size_t sent;
    size_t received;
    size_t dropped;
} UmiProtocolTransportStats;

UmiStatus umi_protocol_transport_create_memory(
    size_t capacity,
    UmiProtocolTransport **out_transport
);
void umi_protocol_transport_destroy(UmiProtocolTransport *transport);
UmiStatus umi_protocol_transport_send(UmiProtocolTransport *transport,
                                      const char *message);
UmiStatus umi_protocol_transport_receive(UmiProtocolTransport *transport,
                                         char *out_message,
                                         size_t capacity);
size_t umi_protocol_transport_count(const UmiProtocolTransport *transport);
UmiProtocolTransportStats umi_protocol_transport_stats(
    const UmiProtocolTransport *transport
);

#ifdef __cplusplus
}
#endif

#endif
