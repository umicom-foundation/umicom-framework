/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_message_model.c
 *
 * PURPOSE:
 *   Verify canonical message initialisation, validation, identifiers and owned deep copies.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiMessageEnvelope message;
    UmiOwnedMessage copy;
    umi_message_envelope_init(&message, UMI_MESSAGE_COMMAND, "studio.open", "a.c");
    assert(message.message_id != 0U);
    assert(message.payload_size == 3U);
    assert(umi_message_validate(&message) == UMI_STATUS_OK);
    assert(umi_message_copy(&message, &copy) == UMI_STATUS_OK);
    assert(strcmp(copy.envelope.name, "studio.open") == 0);
    assert(strcmp(copy.envelope.payload, "a.c") == 0);
    assert(strcmp(umi_message_kind_text(copy.envelope.kind), "command") == 0);
    umi_message_dispose(&copy);
    return 0;
}
