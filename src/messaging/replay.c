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

UmiReplayOptions umi_replay_options_default(void)
{
    UmiReplayOptions options;
    (void)memset(&options, 0, sizeof(options));
    options.maximum_sequence = UINT64_MAX;
    options.maximum_messages = SIZE_MAX;
    return options;
}

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
    if (source == NULL || source->count == NULL || source->read == NULL ||
        handler == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    effective = options != NULL ? *options : umi_replay_options_default();
    count = source->count(source->instance);
    for (index = 0U; index < count && replayed < effective.maximum_messages;
         ++index) {
        UmiOwnedMessage owned;
        UmiStatus status;
        (void)memset(&owned, 0, sizeof(owned));
        status = source->read(source->instance, index, &owned);
        if (status != UMI_STATUS_OK) return status;
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
        if (status != UMI_STATUS_OK) return status;
        replayed++;
    }
    if (out_replayed != NULL) *out_replayed = replayed;
    return UMI_STATUS_OK;
}
