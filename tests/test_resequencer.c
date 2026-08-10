/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_resequencer.c
 *
 * PURPOSE:
 *   Verify out-of-order buffering and deterministic consecutive release.
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
    UmiResequencer *resequencer = NULL;
    UmiOwnedMessage output;
    UmiMessageEnvelope second = make_message("second", 2U, 1U);
    UmiMessageEnvelope first = make_message("first", 1U, 1U);
    assert(umi_resequencer_create(1U, 4U, &resequencer) == UMI_STATUS_OK);
    assert(umi_resequencer_add(resequencer, &second) == UMI_STATUS_OK);
    assert(umi_resequencer_next(resequencer, &output) == UMI_STATUS_NOT_FOUND);
    assert(umi_resequencer_add(resequencer, &first) == UMI_STATUS_OK);
    assert(umi_resequencer_next(resequencer, &output) == UMI_STATUS_OK);
    assert(output.envelope.sequence == 1U);
    umi_message_dispose(&output);
    assert(umi_resequencer_next(resequencer, &output) == UMI_STATUS_OK);
    assert(output.envelope.sequence == 2U);
    umi_message_dispose(&output);
    umi_resequencer_destroy(resequencer);
    return 0;
}
