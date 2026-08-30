/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/transport.c
 *
 * PURPOSE:
 *   Implement the in-memory transport adapter over a bounded channel and common transport disposal semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/transport.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiMemoryTransport {
    UmiChannel *channel;
} UmiMemoryTransport;

static UmiStatus memory_send(void *instance,
                             const UmiMessageEnvelope *message)
{
    UmiMemoryTransport *transport = (UmiMemoryTransport *)instance;
    return transport != NULL
        ? umi_channel_push(transport->channel, message)
        : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus memory_receive(void *instance, UmiOwnedMessage *out_message)
{
    UmiMemoryTransport *transport = (UmiMemoryTransport *)instance;
    return transport != NULL
        ? umi_channel_pop(transport->channel, out_message)
        : UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus memory_close(void *instance)
{
    UmiMemoryTransport *transport = (UmiMemoryTransport *)instance;
    return transport != NULL
        ? umi_channel_close(transport->channel)
        : UMI_STATUS_INVALID_ARGUMENT;
}

static void memory_destroy(void *instance)
{
    UmiMemoryTransport *transport = (UmiMemoryTransport *)instance;
    if (transport == NULL) return;
    umi_channel_destroy(transport->channel);
    free(transport);
}

UmiStatus umi_memory_transport_create(size_t capacity,
                                      UmiTransport *out_transport)
{
    UmiMemoryTransport *instance;
    UmiChannelConfig config = umi_channel_config_default();
    UmiStatus status;
    if (capacity == 0U || out_transport == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_transport, 0, sizeof(*out_transport));
    instance = (UmiMemoryTransport *)calloc(1U, sizeof(*instance));
    if (instance == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    config.capacity = capacity;
    status = umi_channel_create(&config, &instance->channel);
    if (status != UMI_STATUS_OK) {
        free(instance);
        return status;
    }
    out_transport->structure_size = (uint32_t)sizeof(*out_transport);
    out_transport->name = "memory";
    out_transport->instance = instance;
    out_transport->send = memory_send;
    out_transport->receive = memory_receive;
    out_transport->close = memory_close;
    out_transport->destroy = memory_destroy;
    return UMI_STATUS_OK;
}

void umi_transport_dispose(UmiTransport *transport)
{
    if (transport == NULL) return;
    if (transport->destroy != NULL && transport->instance != NULL) {
        transport->destroy(transport->instance);
    }
    (void)memset(transport, 0, sizeof(*transport));
}
