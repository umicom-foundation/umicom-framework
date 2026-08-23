/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_checkpoint_tools.c
 *
 * PURPOSE:
 *   Focused behavior coverage for AI coding tools checkpoint tools.
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
    UmiAiCodingToolPolicy policy;
    UmiAiCodingToolCall call = {0};
    UmiAiCodingToolResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);
    assert(test_workspace_add(
        &f.workspace_storage, "src/a.c", "int a;\n") == UMI_STATUS_OK);

    policy = f.environment.policy;
    policy.allowed_capabilities |= UMI_AI_CODING_TOOL_CAP_CHECKPOINT;
    policy.allow_checkpoint_restore = 1;
    assert(umi_ai_coding_tool_environment_set_policy(
        &f.environment, &policy) == UMI_STATUS_OK);

    call.call_id = 3U;
    call.approved = 1;
    (void)strcpy(call.tool_id, "workspace.checkpoint-create");
    (void)strcpy(
        call.arguments_json,
        "{\"id\":\"c1\",\"label\":\"checkpoint\",\"paths\":[\"src/a.c\"]}");

    assert(umi_ai_coding_tool_execute(
        &f.executor, &call, &result) == UMI_STATUS_OK);
    assert(f.checkpoints.count == 1U);

    tool_test_fixture_deinit(&f);
    return 0;
}

