/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_agent_loop_tool_call.c
 *
 * PURPOSE:
 *   Focused coding-chat/tool-loop coverage for agent loop tool call.
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
        "UMICOM-TOOL-CALL/1\n"
        "TOOL|workspace.exists\n"
        "APPROVED|0\n"
        "ARGUMENTS-BEGIN\n"
        "{\"path\":\"src/main.c\"}\n"
        "ARGUMENTS-END\n";
    provider.responses[1] = "Confirmed: src/main.c exists.";
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
        "Does src/main.c exist?",
        &result) == UMI_STATUS_OK);

    assert(result.tool_calls == 1U);
    assert(result.provider_turns == 2U);
    assert(strstr(result.last_tool_result.output, "\"exists\":true") != NULL);
    assert(strstr(result.final_text, "exists") != NULL);

    tool_test_fixture_deinit(&f);
    return 0;
}

