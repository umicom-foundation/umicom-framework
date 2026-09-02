/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/replay.c
 *
 * PURPOSE:
 *   Implement bounded replay filtering by sequence and correlation while marking replayed messages and preserving ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/replay.h"

#include <string.h>

/*
 * Provide the replay options default operation used by this module and its client
 * applications.
 */
UmiReplayOptions umi_replay_options_default(void)
{
    UmiReplayOptions options;
    (void)memset(&options, 0, sizeof(options));
    options.maximum_sequence = UINT64_MAX;
    options.maximum_messages = SIZE_MAX;
    return options;
}

/*
 * Perform replay through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_replay_execute(const UmiReplaySource *source,
                             const UmiReplayOptions *options,
                             UmiReplayHandler handler,
                             void *user_data,
                             size_t *out_replayed)
{
    UmiReplayOptions effective;
    size_t index;
    size_t replayed = 0U;
    size_t count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL || source->count == NULL || source->read == NULL ||
        handler == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = options != NULL ? *options : umi_replay_options_default();
    count = source->count(source->instance);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count && replayed < effective.maximum_messages;
         ++index) {
        UmiOwnedMessage owned;
        UmiStatus status;
        (void)memset(&owned, 0, sizeof(owned));
        status = source->read(source->instance, index, &owned);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (owned.envelope.sequence < effective.minimum_sequence ||
            owned.envelope.sequence > effective.maximum_sequence ||
            (effective.correlation_id != 0U &&
             owned.envelope.correlation_id != effective.correlation_id)) {
            umi_message_dispose(&owned);
            continue;
        }
        owned.envelope.flags |= UMI_MESSAGE_FLAG_REPLAY;
        status = handler(&owned.envelope, user_data);
        umi_message_dispose(&owned);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        replayed++;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_replayed != NULL) *out_replayed = replayed;
    return UMI_STATUS_OK;
}
