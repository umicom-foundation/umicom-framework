/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_message.c
 *
 * PURPOSE:
 *   Verify DAP request construction and response/event classification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/message.h"

int main(void)
{
    char json[1024];
    UmiDebugRuntimeEnvelope envelope;

    assert(umi_debug_runtime_build_request(
        3U, "threads", NULL, json, sizeof(json)) == UMI_STATUS_OK);
    assert(strstr(json, "\"seq\":3") != NULL);
    assert(strstr(json, "\"command\":\"threads\"") != NULL);

    assert(umi_debug_runtime_message_parse(
        "{\"seq\":9,\"type\":\"response\",\"request_seq\":3,"
        "\"success\":true,\"command\":\"threads\",\"body\":{}}",
        &envelope) == UMI_STATUS_OK);
    assert(envelope.kind == UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE);
    assert(envelope.request_sequence == 3U);
    assert(envelope.success == 1);

    assert(umi_debug_runtime_message_parse(
        "{\"seq\":10,\"type\":\"event\",\"event\":\"stopped\","
        "\"body\":{\"reason\":\"breakpoint\"}}",
        &envelope) == UMI_STATUS_OK);
    assert(envelope.kind == UMI_DEBUG_RUNTIME_MESSAGE_EVENT);
    assert(strcmp(envelope.event, "stopped") == 0);
    return 0;
}
