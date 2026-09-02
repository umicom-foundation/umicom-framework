/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/messaging/replay.h
 *
 * PURPOSE:
 *   Declare deployment-neutral replay sources, cursor constraints and callbacks for deterministic reprocessing of durable messages.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MESSAGING_REPLAY_H
#define UMICOM_MESSAGING_REPLAY_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/messaging/message.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the replay source data shared with callers of this public contract.
 */
typedef struct UmiReplaySource {
    void *instance;
    size_t (*count)(void *instance);
    UmiStatus (*read)(void *instance,
                      size_t index,
                      UmiOwnedMessage *out_message);
} UmiReplaySource;

/**
 * Represent the replay options data shared with callers of this public contract.
 */
typedef struct UmiReplayOptions {
    uint64_t minimum_sequence;
    uint64_t maximum_sequence;
    uint64_t correlation_id;
    size_t maximum_messages;
} UmiReplayOptions;

typedef UmiStatus (*UmiReplayHandler)(const UmiMessageEnvelope *message,
                                      void *user_data);

/**
 * Provide the replay options default operation used by this module and its client
 * applications.
 */
UmiReplayOptions umi_replay_options_default(void);
/**
 * Perform replay through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_replay_execute(const UmiReplaySource *source,
                             const UmiReplayOptions *options,
                             UmiReplayHandler handler,
                             void *user_data,
                             size_t *out_replayed);

#ifdef __cplusplus
}
#endif

#endif
