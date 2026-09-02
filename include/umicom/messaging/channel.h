/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/channel.h
 *
 * PURPOSE:
 *   Declare a bounded owned-message channel with backpressure, close semantics and observable queue statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the channel data shared with callers of this public contract.
 */
typedef struct UmiChannel UmiChannel;

/**
 * Represent the channel config data shared with callers of this public contract.
 */
typedef struct UmiChannelConfig {
    size_t capacity;
    int reject_when_full;
} UmiChannelConfig;

/**
 * Represent the channel stats data shared with callers of this public contract.
 */
typedef struct UmiChannelStats {
    size_t capacity;
    size_t queued;
    uint64_t accepted;
    uint64_t delivered;
    uint64_t rejected;
    int closed;
} UmiChannelStats;

/**
 * Provide the channel config default operation used by this module and its client
 * applications.
 */
UmiChannelConfig umi_channel_config_default(void);
/**
 * Initialise channel from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_channel_create(const UmiChannelConfig *config,
                             UmiChannel **out_channel);
/**
 * Release or reset state held by channel so the same storage can be reused safely.
 */
void umi_channel_destroy(UmiChannel *channel);
/**
 * Provide the channel push operation used by this module and its client applications.
 */
UmiStatus umi_channel_push(UmiChannel *channel,
                           const UmiMessageEnvelope *message);
/**
 * Provide the channel pop operation used by this module and its client applications.
 */
UmiStatus umi_channel_pop(UmiChannel *channel, UmiOwnedMessage *out_message);
/**
 * Provide the channel close operation used by this module and its client applications.
 */
UmiStatus umi_channel_close(UmiChannel *channel);
/**
 * Provide the channel stats operation used by this module and its client applications.
 */
UmiChannelStats umi_channel_stats(const UmiChannel *channel);

#ifdef __cplusplus
}
#endif

#endif
