/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_platform_policy_restore.c
 *
 * PURPOSE:
 *   Focused AI coding tools platform coverage for platform policy restore.
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

    umi_ai_runtime_init(&runtime);
    test_workspace_init(&storage);
    assert(test_workspace_adapter(&storage, &workspace) == UMI_STATUS_OK);
    assert(runtime.policy.allow_tools == 0);

    assert(umi_ai_coding_tools_platform_create(
        &runtime, NULL, &workspace, ".", NULL, &platform) == UMI_STATUS_OK);
    assert(runtime.policy.allow_tools == 1);
    assert(runtime.policy.require_tool_approval == 1);

    umi_ai_coding_tools_platform_destroy(platform);
    assert(runtime.policy.allow_tools == 0);
    return 0;
}

