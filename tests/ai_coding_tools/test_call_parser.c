/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_call_parser.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI coding tools call parser.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_tools/call_parser.h"

int main(void)
{
    UmiAiCodingToolCall call;

    assert(umi_ai_coding_tool_call_parse(
        "UMICOM-TOOL-CALL/1\n"
        "TOOL|workspace.exists\n"
        "APPROVED|0\n"
        "ARGUMENTS-BEGIN\n"
        "{\"path\":\"src/main.c\"}\n"
        "ARGUMENTS-END\n",
        9U,
        &call) == UMI_STATUS_OK);

    assert(call.call_id == 9U);
    assert(strcmp(call.tool_id, "workspace.exists") == 0);
    assert(strstr(call.arguments_json, "src/main.c") != NULL);
    assert(call.approved == 0);
    return 0;
}

