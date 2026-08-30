/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_agent_loop_approval_stop.c
 *
 * PURPOSE:
 *   Focused coding-chat/tool-loop coverage for agent loop approval stop.
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
    ToolTestFixture f;
    FakeProviderState provider = {0};
    UmiAiCodingToolPolicy policy;
    UmiAiCodingToolChatSession session;
    UmiAiCodingToolLoopConfig config;
    UmiAiCodingToolLoopResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);

    policy = f.environment.policy;
    policy.allowed_capabilities |= UMI_AI_CODING_TOOL_CAP_PROCESS_EXECUTION;
    policy.allow_process_execution = 1;
    assert(umi_ai_coding_tool_environment_set_policy(
        &f.environment, &policy) == UMI_STATUS_OK);

    provider.responses[0] =
        "UMICOM-TOOL-CALL/1\n"
        "TOOL|developer.build\n"
        "APPROVED|0\n"
        "ARGUMENTS-BEGIN\n"
        "{\"program\":\"cmake\",\"arguments\":\"--build build\"}\n"
        "ARGUMENTS-END\n";
    provider.response_count = 1U;

    assert(tool_test_add_provider(
        &f.runtime, "test.provider", &provider) == UMI_STATUS_OK);
    assert(umi_ai_coding_tool_chat_session_init(
        &session, "chat", "test.provider", "test-model") == UMI_STATUS_OK);
    umi_ai_coding_tool_loop_config_init(&config);

    assert(umi_ai_coding_tool_agent_loop_run(
        &f.runtime,
        &f.environment,
        &f.executor,
        &session,
        &config,
        "Build the project.",
        &result) == UMI_STATUS_PERMISSION_DENIED);

    assert(result.approval_stops == 1U);
    assert(result.last_tool_result.state ==
           UMI_AI_CODING_TOOL_CALL_APPROVAL_REQUIRED);

    tool_test_fixture_deinit(&f);
    return 0;
}

