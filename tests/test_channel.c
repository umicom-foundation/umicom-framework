/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_channel.c
 *
 * PURPOSE:
 *   Verify bounded channel ownership, backpressure, delivery and close semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>


static UmiMessageEnvelope make_message(const char *name,
                                       uint64_t sequence,
                                       uint64_t correlation)
{
    UmiMessageEnvelope message;
    umi_message_envelope_init(&message, UMI_MESSAGE_EVENT, name, "payload");
    message.sequence = sequence;
    message.correlation_id = correlation;
    message.schema_id = "test.event";
    message.schema_version = 1U;
    return message;
}

int main(void)
{
    UmiChannel *channel = NULL;
    UmiChannelConfig config = {2U, 1};
    UmiOwnedMessage message;
    UmiMessageEnvelope first = make_message("one", 1U, 1U);
    UmiMessageEnvelope second = make_message("two", 2U, 1U);
    UmiMessageEnvelope third = make_message("three", 3U, 1U);
    assert(umi_channel_create(&config, &channel) == UMI_STATUS_OK);
    assert(umi_channel_push(channel, &first) == UMI_STATUS_OK);
    assert(umi_channel_push(channel, &second) == UMI_STATUS_OK);
    assert(umi_channel_push(channel, &third) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(umi_channel_pop(channel, &message) == UMI_STATUS_OK);
    assert(message.envelope.sequence == 1U);
    umi_message_dispose(&message);
    assert(umi_channel_close(channel) == UMI_STATUS_OK);
    umi_channel_destroy(channel);
    return 0;
}
