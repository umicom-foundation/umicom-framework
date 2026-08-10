/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_protocol_framing.c
 *
 * PURPOSE:
 *   Verify Content-Length framing and payload recovery.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/protocol/framing.h"

int main(void)
{
    char buffer[512];
    char content[256];
    UmiProtocolFrame frame;
    const char *json = "{\\\"jsonrpc\\\":\\\"2.0\\\"}";
    assert(umi_protocol_frame_encode(json, buffer, sizeof(buffer), NULL) == UMI_STATUS_OK);
    assert(umi_protocol_frame_parse(buffer, strlen(buffer), &frame) == UMI_STATUS_OK);
    assert(frame.content_length == strlen(json));
    assert(umi_protocol_frame_copy_content(&frame, content, sizeof(content)) == UMI_STATUS_OK);
    assert(strcmp(content, json) == 0);
    return 0;
}
