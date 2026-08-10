/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_replay.c
 *
 * PURPOSE:
 *   Verify replay filtering, replay flagging and bounded replay counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

static UmiOwnedMessage messages[3];
static size_t source_count(void *instance) { (void)instance; return 3U; }
static UmiStatus source_read(void *instance, size_t index,
                             UmiOwnedMessage *out_message)
{
    (void)instance;
    return umi_message_copy(&messages[index].envelope, out_message);
}
static UmiStatus replay_handler(const UmiMessageEnvelope *message, void *user_data)
{
    size_t *count = (size_t *)user_data;
    assert((message->flags & UMI_MESSAGE_FLAG_REPLAY) != 0U);
    (*count)++;
    return UMI_STATUS_OK;
}
int main(void)
{
    UmiReplaySource source = {NULL, source_count, source_read};
    UmiReplayOptions options = umi_replay_options_default();
    UmiMessageEnvelope message;
    size_t count = 0U;
    size_t replayed = 0U;
    size_t index;
    for (index = 0U; index < 3U; ++index) {
        umi_message_envelope_init(&message, UMI_MESSAGE_EVENT, "e", "p");
        message.sequence = index + 1U;
        message.correlation_id = index < 2U ? 7U : 9U;
        assert(umi_message_copy(&message, &messages[index]) == UMI_STATUS_OK);
    }
    options.correlation_id = 7U;
    assert(umi_replay_execute(&source, &options, replay_handler,
                              &count, &replayed) == UMI_STATUS_OK);
    assert(count == 2U && replayed == 2U);
    for (index = 0U; index < 3U; ++index) umi_message_dispose(&messages[index]);
    return 0;
}
