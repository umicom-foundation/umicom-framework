/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/protocol/transport.c
 *
 * PURPOSE:
 *   Implement a bounded thread-safe in-memory protocol transport for tests and in-process language services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/protocol/transport.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

struct UmiProtocolTransport {
    char (*messages)[UMI_PROTOCOL_MESSAGE_CAPACITY];
    size_t capacity;
    size_t count;
    size_t head;
    UmiProtocolTransportStats stats;
    UmiMutex *mutex;
};

UmiStatus umi_protocol_transport_create_memory(
    size_t capacity,
    UmiProtocolTransport **out_transport)
{
    UmiProtocolTransport *transport;
    if (out_transport == NULL || capacity == 0U ||
        capacity > UMI_PROTOCOL_MAX_QUEUE) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_transport = NULL;
    transport = (UmiProtocolTransport *)calloc(1U, sizeof(*transport));
    if (transport == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    transport->messages = calloc(capacity,
                                 sizeof(transport->messages[0]));
    if (transport->messages == NULL ||
        umi_mutex_create(&transport->mutex) != UMI_STATUS_OK) {
        umi_protocol_transport_destroy(transport);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    transport->capacity = capacity;
    *out_transport = transport;
    return UMI_STATUS_OK;
}

void umi_protocol_transport_destroy(UmiProtocolTransport *transport)
{
    if (transport != NULL) {
        umi_mutex_destroy(transport->mutex);
        free(transport->messages);
        free(transport);
    }
}

UmiStatus umi_protocol_transport_send(UmiProtocolTransport *transport,
                                      const char *message)
{
    size_t position;
    if (transport == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(message) + 1U > UMI_PROTOCOL_MESSAGE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)umi_mutex_lock(transport->mutex);
    if (transport->count >= transport->capacity) {
        transport->stats.dropped += 1U;
        (void)umi_mutex_unlock(transport->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    position = (transport->head + transport->count) %
               transport->capacity;
    (void)snprintf(transport->messages[position],
                   UMI_PROTOCOL_MESSAGE_CAPACITY,
                   "%s",
                   message);
    transport->count += 1U;
    transport->stats.queued = transport->count;
    transport->stats.sent += 1U;
    (void)umi_mutex_unlock(transport->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_protocol_transport_receive(UmiProtocolTransport *transport,
                                         char *out_message,
                                         size_t capacity)
{
    size_t length;
    if (transport == NULL || out_message == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(transport->mutex);
    if (transport->count == 0U) {
        (void)umi_mutex_unlock(transport->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    length = strlen(transport->messages[transport->head]);
    if (length + 1U > capacity) {
        (void)umi_mutex_unlock(transport->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(out_message,
                 transport->messages[transport->head],
                 length + 1U);
    transport->messages[transport->head][0] = '\0';
    transport->head = (transport->head + 1U) %
                      transport->capacity;
    transport->count -= 1U;
    transport->stats.queued = transport->count;
    transport->stats.received += 1U;
    (void)umi_mutex_unlock(transport->mutex);
    return UMI_STATUS_OK;
}

size_t umi_protocol_transport_count(const UmiProtocolTransport *transport)
{
    return transport != NULL ? transport->count : 0U;
}

UmiProtocolTransportStats umi_protocol_transport_stats(
    const UmiProtocolTransport *transport)
{
    UmiProtocolTransportStats stats = {0U, 0U, 0U, 0U};
    if (transport != NULL) {
        stats = transport->stats;
    }
    return stats;
}
