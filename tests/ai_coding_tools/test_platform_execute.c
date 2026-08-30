/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_platform_execute.c
 *
 * PURPOSE:
 *   Focused AI coding tools platform coverage for platform execute.
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

int main(void)
{
    UmiAiRuntime runtime;
    TestWorkspace storage;
    UmiAiCodingWorkspaceAdapter workspace;
    UmiAiCodingToolsPlatform *platform = NULL;
    UmiAiCodingToolCall call = {0};
    UmiAiCodingToolResult result;
    UmiAiCodingToolsPlatformSnapshot snapshot;

    umi_ai_runtime_init(&runtime);
    test_workspace_init(&storage);
    assert(test_workspace_add(
        &storage, "src/a.c", "int a;\n") == UMI_STATUS_OK);
    assert(test_workspace_adapter(&storage, &workspace) == UMI_STATUS_OK);

    assert(umi_ai_coding_tools_platform_create(
        &runtime, NULL, &workspace, ".", NULL, &platform) == UMI_STATUS_OK);

    call.call_id = 1U;
    (void)strcpy(call.tool_id, "workspace.exists");
    (void)strcpy(call.arguments_json, "{\"path\":\"src/a.c\"}");

    assert(umi_ai_coding_tools_platform_execute(
        platform, &call, &result) == UMI_STATUS_OK);
    assert(strstr(result.output, "\"exists\":true") != NULL);

    assert(umi_ai_coding_tools_platform_snapshot(
        platform, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.result_history_count == 1U);

    umi_ai_coding_tools_platform_destroy(platform);
    return 0;
}

