/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_plan_executor.c
 *
 * PURPOSE:
 *   Focused integration coverage for AI coding tools plan executor.
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
    UmiAiCodingToolPlan plan;
    UmiAiCodingToolPlanStep step = {0};
    UmiAiCodingToolPlanResult result;

    assert(tool_test_fixture_init(&f) == UMI_STATUS_OK);
    assert(test_workspace_add(
        &f.workspace_storage, "src/a.c", "int a;\n") == UMI_STATUS_OK);

    assert(umi_ai_coding_tool_plan_init(
        &plan, "inspect", "Inspect", "Read before reasoning.") == UMI_STATUS_OK);

    (void)strcpy(step.step_id, "exists");
    step.call.call_id = 20U;
    (void)strcpy(step.call.tool_id, "workspace.exists");
    (void)strcpy(step.call.arguments_json, "{\"path\":\"src/a.c\"}");
    step.required = 1;
    assert(umi_ai_coding_tool_plan_add(&plan, &step) == UMI_STATUS_OK);

    (void)memset(&step, 0, sizeof(step));
    (void)strcpy(step.step_id, "read");
    step.call.call_id = 21U;
    (void)strcpy(step.call.tool_id, "workspace.read");
    (void)strcpy(step.call.arguments_json, "{\"path\":\"src/a.c\"}");
    step.required = 1;
    step.has_dependency = 1;
    step.depends_on_index = 0U;
    assert(umi_ai_coding_tool_plan_add(&plan, &step) == UMI_STATUS_OK);

    assert(umi_ai_coding_tool_plan_execute(
        &f.executor, &plan, &result) == UMI_STATUS_OK);
    assert(result.succeeded);
    assert(result.passed_count == 2U);

    tool_test_fixture_deinit(&f);
    return 0;
}

