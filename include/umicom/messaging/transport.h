/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/transport.h
 *
 * PURPOSE:
 *   Define a deployment-neutral message transport function table and a deterministic in-memory transport for tests and modular-monolith deployments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_TRANSPORT_H
#define UMICOM_MESSAGING_TRANSPORT_H

#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/channel.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTransport {
    uint32_t structure_size;
    const char *name;
    void *instance;
    UmiStatus (*send)(void *instance, const UmiMessageEnvelope *message);
    UmiStatus (*receive)(void *instance, UmiOwnedMessage *out_message);
    UmiStatus (*close)(void *instance);
    void (*destroy)(void *instance);
} UmiTransport;

UmiStatus umi_memory_transport_create(size_t capacity,
                                      UmiTransport *out_transport);
void umi_transport_dispose(UmiTransport *transport);

#ifdef __cplusplus
}
#endif

#endif
