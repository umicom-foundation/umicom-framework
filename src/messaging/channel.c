/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/channel.c
 *
 * PURPOSE:
 *   Implement a bounded ring channel that owns queued messages and reports accepted, delivered and rejected traffic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/channel.h"

#include <stdlib.h>
#include <string.h>

#include "include/messaging_internal.h"

struct UmiChannel {
    UmiSpinLock lock;
    UmiOwnedMessage *items;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
    uint64_t accepted;
    uint64_t delivered;
    uint64_t rejected;
    int reject_when_full;
    int closed;
};

UmiChannelConfig umi_channel_config_default(void)
{
    UmiChannelConfig config;
    config.capacity = 256U;
    config.reject_when_full = 1;
    return config;
}

UmiStatus umi_channel_create(const UmiChannelConfig *config,
                             UmiChannel **out_channel)
{
    UmiChannelConfig effective;
    UmiChannel *channel;
    if (out_channel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_channel = NULL;
    effective = config != NULL ? *config : umi_channel_config_default();
    if (effective.capacity == 0U || effective.capacity > 65536U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    channel = (UmiChannel *)calloc(1U, sizeof(*channel));
    if (channel == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    channel->items = (UmiOwnedMessage *)calloc(effective.capacity,
                                               sizeof(*channel->items));
    if (channel->items == NULL) {
        free(channel);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_spin_init(&channel->lock);
    channel->capacity = effective.capacity;
    channel->reject_when_full = effective.reject_when_full;
    *out_channel = channel;
    return UMI_STATUS_OK;
}

void umi_channel_destroy(UmiChannel *channel)
{
    size_t index;
    if (channel == NULL) return;
    for (index = 0U; index < channel->capacity; ++index) {
        umi_message_dispose(&channel->items[index]);
    }
    free(channel->items);
    free(channel);
}

UmiStatus umi_channel_push(UmiChannel *channel,
                           const UmiMessageEnvelope *message)
{
    UmiOwnedMessage copy;
    UmiStatus status;
    if (channel == NULL || message == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_message_copy(message, &copy);
    if (status != UMI_STATUS_OK) return status;
    umi_spin_lock(&channel->lock);
    if (channel->closed) {
        umi_spin_unlock(&channel->lock);
        umi_message_dispose(&copy);
        return UMI_STATUS_INVALID_STATE;
    }
    if (channel->count == channel->capacity) {
        channel->rejected++;
        umi_spin_unlock(&channel->lock);
        umi_message_dispose(&copy);
        return channel->reject_when_full
            ? UMI_STATUS_CAPACITY_EXCEEDED
            : UMI_STATUS_BUSY;
    }
    channel->items[channel->tail] = copy;
    channel->tail = (channel->tail + 1U) % channel->capacity;
    channel->count++;
    channel->accepted++;
    umi_spin_unlock(&channel->lock);
    return UMI_STATUS_OK;
}

UmiStatus umi_channel_pop(UmiChannel *channel, UmiOwnedMessage *out_message)
{
    if (channel == NULL || out_message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_message, 0, sizeof(*out_message));
    umi_spin_lock(&channel->lock);
    if (channel->count == 0U) {
        UmiStatus status = channel->closed
            ? UMI_STATUS_INVALID_STATE
            : UMI_STATUS_NOT_FOUND;
        umi_spin_unlock(&channel->lock);
        return status;
    }
    *out_message = channel->items[channel->head];
    (void)memset(&channel->items[channel->head],
                 0,
                 sizeof(channel->items[channel->head]));
    channel->head = (channel->head + 1U) % channel->capacity;
    channel->count--;
    channel->delivered++;
    umi_spin_unlock(&channel->lock);
    return UMI_STATUS_OK;
}

UmiStatus umi_channel_close(UmiChannel *channel)
{
    if (channel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_spin_lock(&channel->lock);
    channel->closed = 1;
    umi_spin_unlock(&channel->lock);
    return UMI_STATUS_OK;
}

UmiChannelStats umi_channel_stats(const UmiChannel *channel)
{
    UmiChannelStats stats;
    (void)memset(&stats, 0, sizeof(stats));
    if (channel == NULL) return stats;
    stats.capacity = channel->capacity;
    stats.queued = channel->count;
    stats.accepted = channel->accepted;
    stats.delivered = channel->delivered;
    stats.rejected = channel->rejected;
    stats.closed = channel->closed;
    return stats;
}
