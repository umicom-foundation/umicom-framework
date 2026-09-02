/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/integration_test_051.c
 *
 * PURPOSE:
 *   Validate canonical context identity, colour mapping and bounded value handling across product-neutral scenarios.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/context_channel/context_channel.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiContextPayload payload;
    UmiContextValue value;
    UmiContextColourToken token;
    char context_id[UMI_CONTEXT_TEXT_CAPACITY];
    (void)umi_context_copy_text(context_id,sizeof(context_id),"integration-051");
    umi_context_payload_init(&payload,UMI_CONTEXT_KIND_TRADE,context_id,"umicom.context/integration");
    umi_context_value_init(&value,"scenario");
    assert(umi_context_value_set_unsigned(&value,51U) == UMI_STATUS_OK);
    assert(umi_context_payload_add_value(&payload,&value) == UMI_STATUS_OK);
    assert(umi_context_colour_token_for(UMI_CONTEXT_COLOUR_GREEN,&token) == UMI_STATUS_OK);
    assert(token.css_class[0] != '\0');
    assert(payload.content_hash != 0U);
    return 0;
}
