/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_agent_loop_final.c
 *
 * PURPOSE:
 *   Focused coding-chat/tool-loop coverage for agent loop final.
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
    FakeProviderState provider = {0};
    UmiAiCodingToolChatSession session;
    UmiAiCodingToolLoopConfig config;
    UmiAiCodingToolLoopResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);

    provider.responses[0] = "Final repository-aware answer.";
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
        "Explain the repository.",
        &result) == UMI_STATUS_OK);

    assert(result.completed);
    assert(result.provider_turns == 1U);
    assert(strcmp(result.final_text, "Final repository-aware answer.") == 0);

    tool_test_fixture_deinit(&f);
    return 0;
}

