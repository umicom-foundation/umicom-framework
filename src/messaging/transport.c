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

/* Provide the memory send operation used by this module and its client applications. */
static UmiStatus memory_send(void *instance,
                             const UmiMessageEnvelope *message)
{
    UmiMemoryTransport *transport = (UmiMemoryTransport *)instance;
    return transport != NULL
        ? umi_channel_push(transport->channel, message)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Provide the memory receive operation used by this module and its client applications. */
static UmiStatus memory_receive(void *instance, UmiOwnedMessage *out_message)
{
    UmiMemoryTransport *transport = (UmiMemoryTransport *)instance;
    return transport != NULL
        ? umi_channel_pop(transport->channel, out_message)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Provide the memory close operation used by this module and its client applications. */
static UmiStatus memory_close(void *instance)
{
    UmiMemoryTransport *transport = (UmiMemoryTransport *)instance;
    return transport != NULL
        ? umi_channel_close(transport->channel)
        : UMI_STATUS_INVALID_ARGUMENT;
}

/* Release or reset state held by memory so the same storage can be reused safely. */
static void memory_destroy(void *instance)
{
    UmiMemoryTransport *transport = (UmiMemoryTransport *)instance;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transport == NULL) return;
    umi_channel_destroy(transport->channel);
    free(transport);
}

/*
 * Initialise memory transport from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_memory_transport_create(size_t capacity,
                                      UmiTransport *out_transport)
{
    UmiMemoryTransport *instance;
    UmiChannelConfig config = umi_channel_config_default();
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capacity == 0U || out_transport == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_transport, 0, sizeof(*out_transport));
    instance = (UmiMemoryTransport *)calloc(1U, sizeof(*instance));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (instance == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    config.capacity = capacity;
    status = umi_channel_create(&config, &instance->channel);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/* Release or reset state held by transport so the same storage can be reused safely. */
void umi_transport_dispose(UmiTransport *transport)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transport == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transport->destroy != NULL && transport->instance != NULL) {
        transport->destroy(transport->instance);
    }
    (void)memset(transport, 0, sizeof(*transport));
}
