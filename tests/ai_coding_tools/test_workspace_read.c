/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_workspace_read.c
 *
 * PURPOSE:
 *   Focused behavior coverage for AI coding tools workspace read.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "tool_test_support.h"

int main(void)
{
    ToolTestFixture f;
    UmiAiCodingToolCall call = {0};
    UmiAiCodingToolResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);
    assert(test_workspace_add(
        &f.workspace_storage,
        "src/main.c",
        "int main(void) { return 0; }\n") == UMI_STATUS_OK);

    call.call_id = 1U;
    (void)strcpy(call.tool_id, "workspace.read");
    (void)strcpy(call.arguments_json, "{\"path\":\"src/main.c\"}");

    assert(umi_ai_coding_tool_execute(
        &f.executor, &call, &result) == UMI_STATUS_OK);
    assert(result.state == UMI_AI_CODING_TOOL_CALL_SUCCEEDED);
    assert(strstr(result.output, "int main") != NULL);

    tool_test_fixture_deinit(&f);
    return 0;
}

