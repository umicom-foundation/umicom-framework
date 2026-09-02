/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/transport.h
 *
 * PURPOSE:
 *   Define a deployment-neutral message transport function table and a deterministic in-memory transport for tests and modular-monolith deployments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_TRANSPORT_H
#define UMICOM_MESSAGING_TRANSPORT_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/channel.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the transport data shared with callers of this public contract.
 */
typedef struct UmiTransport {
    uint32_t structure_size;
    const char *name;
    void *instance;
    UmiStatus (*send)(void *instance, const UmiMessageEnvelope *message);
    UmiStatus (*receive)(void *instance, UmiOwnedMessage *out_message);
    UmiStatus (*close)(void *instance);
    void (*destroy)(void *instance);
} UmiTransport;

/**
 * Initialise memory transport from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_memory_transport_create(size_t capacity,
                                      UmiTransport *out_transport);
/**
 * Release or reset state held by transport so the same storage can be reused safely.
 */
void umi_transport_dispose(UmiTransport *transport);

#ifdef __cplusplus
}
#endif

#endif
