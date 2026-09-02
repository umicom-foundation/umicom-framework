/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_agent_loop_plan.c
 *
 * PURPOSE:
 *   Focused coding-chat/tool-loop coverage for agent loop plan.
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
    FakeProviderState provider = {0};
    UmiAiCodingToolChatSession session;
    UmiAiCodingToolLoopConfig config;
    UmiAiCodingToolLoopResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);
    assert(test_workspace_add(
        &f.workspace_storage, "src/main.c", "int main(void){}\n") ==
        UMI_STATUS_OK);

    provider.responses[0] =
        "UMICOM-TOOL-PLAN/1\n"
        "PLAN|inspect|Inspect source\n"
        "RATIONALE-BEGIN\nCheck then read.\nRATIONALE-END\n"
        "STEP-BEGIN|exists|workspace.exists|1|0\n"
        "ARGUMENTS-BEGIN\n{\"path\":\"src/main.c\"}\nARGUMENTS-END\n"
        "STEP-END\n"
        "STEP-BEGIN|read|workspace.read|1|0\n"
        "ARGUMENTS-BEGIN\n{\"path\":\"src/main.c\"}\nARGUMENTS-END\n"
        "STEP-END\n"
        "PLAN-END\n";
    provider.responses[1] = "Inspection complete.";
    provider.response_count = 2U;

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
        "Inspect src/main.c.",
        &result) == UMI_STATUS_OK);

    assert(result.tool_plans == 1U);
    assert(result.last_plan_result.succeeded);
    assert(result.last_plan_result.passed_count == 2U);
    assert(result.completed);

    tool_test_fixture_deinit(&f);
    return 0;
}

