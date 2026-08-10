/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/channel.h
 *
 * PURPOSE:
 *   Declare a bounded owned-message channel with backpressure, close semantics and observable queue statistics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_CHANNEL_H
#define UMICOM_MESSAGING_CHANNEL_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiChannel UmiChannel;

typedef struct UmiChannelConfig {
    size_t capacity;
    int reject_when_full;
} UmiChannelConfig;

typedef struct UmiChannelStats {
    size_t capacity;
    size_t queued;
    uint64_t accepted;
    uint64_t delivered;
    uint64_t rejected;
    int closed;
} UmiChannelStats;

UmiChannelConfig umi_channel_config_default(void);
UmiStatus umi_channel_create(const UmiChannelConfig *config,
                             UmiChannel **out_channel);
void umi_channel_destroy(UmiChannel *channel);
UmiStatus umi_channel_push(UmiChannel *channel,
                           const UmiMessageEnvelope *message);
UmiStatus umi_channel_pop(UmiChannel *channel, UmiOwnedMessage *out_message);
UmiStatus umi_channel_close(UmiChannel *channel);
UmiChannelStats umi_channel_stats(const UmiChannel *channel);

#ifdef __cplusplus
}
#endif

#endif
