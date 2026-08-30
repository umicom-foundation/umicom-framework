/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_platform_chat.c
 *
 * PURPOSE:
 *   Focused AI coding tools platform coverage for platform chat.
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
    FakeProviderState provider = {0};
    TestWorkspace storage;
    UmiAiCodingWorkspaceAdapter workspace;
    UmiAiCodingToolsPlatform *platform = NULL;
    UmiAiCodingToolLoopConfig config;
    UmiAiCodingToolLoopResult result;

    umi_ai_runtime_init(&runtime);
    provider.responses[0] = "Hello from the coding assistant.";
    provider.response_count = 1U;
    assert(tool_test_add_provider(
        &runtime, "test.provider", &provider) == UMI_STATUS_OK);

    test_workspace_init(&storage);
    assert(test_workspace_adapter(&storage, &workspace) == UMI_STATUS_OK);

    assert(umi_ai_coding_tools_platform_create(
        &runtime, NULL, &workspace, ".", NULL, &platform) == UMI_STATUS_OK);
    assert(umi_ai_coding_tools_platform_open_chat(
        platform, "chat", "test.provider", "test-model") == UMI_STATUS_OK);

    umi_ai_coding_tool_loop_config_init(&config);
    assert(umi_ai_coding_tools_platform_run_chat(
        platform,
        "chat",
        &config,
        "Hello",
        &result) == UMI_STATUS_OK);

    assert(result.completed);
    assert(strstr(result.final_text, "Hello") != NULL);

    umi_ai_coding_tools_platform_destroy(platform);
    return 0;
}

