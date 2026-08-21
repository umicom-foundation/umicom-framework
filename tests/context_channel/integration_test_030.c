/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/integration_test_030.c
 *
 * PURPOSE:
 *   Validate canonical context identity, colour mapping and bounded value handling across product-neutral scenarios.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/context_channel/context_channel.h"
int main(void)
{
    UmiContextPayload payload;
    UmiContextValue value;
    UmiContextColourToken token;
    char context_id[UMI_CONTEXT_TEXT_CAPACITY];
    (void)umi_context_copy_text(context_id,sizeof(context_id),"integration-030");
    umi_context_payload_init(&payload,UMI_CONTEXT_KIND_MEDIA,context_id,"umicom.context/integration");
    umi_context_value_init(&value,"scenario");
    assert(umi_context_value_set_unsigned(&value,30U) == UMI_STATUS_OK);
    assert(umi_context_payload_add_value(&payload,&value) == UMI_STATUS_OK);
    assert(umi_context_colour_token_for(UMI_CONTEXT_COLOUR_PURPLE,&token) == UMI_STATUS_OK);
    assert(token.css_class[0] != '\0');
    assert(payload.content_hash != 0U);
    return 0;
}
