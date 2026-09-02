/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_workspace_exists.c
 *
 * PURPOSE:
 *   Focused behavior coverage for AI coding tools workspace exists.
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
#include "tool_test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    ToolTestFixture f;
    UmiAiCodingToolCall call = {0};
    UmiAiCodingToolResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);
    assert(test_workspace_add(
        &f.workspace_storage, "CMakeLists.txt", "project(Test)\n") ==
        UMI_STATUS_OK);

    call.call_id = 2U;
    (void)strcpy(call.tool_id, "workspace.exists");
    (void)strcpy(call.arguments_json, "{\"path\":\"CMakeLists.txt\"}");

    assert(umi_ai_coding_tool_execute(
        &f.executor, &call, &result) == UMI_STATUS_OK);
    assert(strstr(result.output, "\"exists\":true") != NULL);

    tool_test_fixture_deinit(&f);
    return 0;
}

