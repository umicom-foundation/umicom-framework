/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_platform_snapshot.c
 *
 * PURPOSE:
 *   Focused AI coding tools platform coverage for platform snapshot.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "tool_test_support.h"

int main(void)
{
    UmiAiRuntime runtime;
    TestWorkspace storage;
    UmiAiCodingWorkspaceAdapter workspace;
    UmiAiCodingToolsPlatform *platform = NULL;
    UmiAiCodingToolsPlatformSnapshot snapshot;

    umi_ai_runtime_init(&runtime);
    test_workspace_init(&storage);
    assert(test_workspace_adapter(&storage, &workspace) == UMI_STATUS_OK);

    assert(umi_ai_coding_tools_platform_create(
        &runtime, NULL, &workspace, ".", NULL, &platform) == UMI_STATUS_OK);
    assert(umi_ai_coding_tools_platform_snapshot(
        platform, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.tool_count == 40U);
    assert(snapshot.checkpoint_count == 0U);
    assert(snapshot.chat_session_count == 0U);

    umi_ai_coding_tools_platform_destroy(platform);
    return 0;
}

