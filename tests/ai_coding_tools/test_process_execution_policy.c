/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_process_execution_policy.c
 *
 * PURPOSE:
 *   Focused behavior coverage for AI coding tools process execution policy.
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
    UmiDeveloperExecutor developer_executor;
    FakeExecutionState execution = {0};
    UmiAiCodingToolPolicy policy;
    UmiAiCodingToolCall call = {0};
    UmiAiCodingToolResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);

    execution.launched = 1;
    execution.exit_code = 0;
    execution.output = "build ok";
    umi_developer_executor_init(
        &developer_executor, tool_test_fake_execute, &execution);

    assert(umi_ai_coding_tool_environment_set_executor(
        &f.environment, &developer_executor) == UMI_STATUS_OK);

    call.call_id = 4U;
    call.approved = 1;
    (void)strcpy(call.tool_id, "developer.build");
    (void)strcpy(
        call.arguments_json,
        "{\"program\":\"cmake\",\"arguments\":\"--build build\"}");

    assert(umi_ai_coding_tool_execute(
        &f.executor, &call, &result) == UMI_STATUS_PERMISSION_DENIED);

    policy = f.environment.policy;
    policy.allowed_capabilities |= UMI_AI_CODING_TOOL_CAP_PROCESS_EXECUTION;
    policy.allow_process_execution = 1;
    assert(umi_ai_coding_tool_environment_set_policy(
        &f.environment, &policy) == UMI_STATUS_OK);

    assert(umi_ai_coding_tool_execute(
        &f.executor, &call, &result) == UMI_STATUS_OK);
    assert(execution.calls == 1U);
    assert(strstr(result.output, "build ok") != NULL);

    tool_test_fixture_deinit(&f);
    return 0;
}

