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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The original automatic fixture can exceed the native Windows stack before its first assertion. The replacement owns the same records on the heap; retain the earlier test and its comments for review. */
#if 0
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


#endif
#include <stdlib.h>
#include <stdio.h>

/* Large bounded records belong to this explicit fixture owner, not the native
 * thread stack. Each process still creates a fresh independent model and runs
 * the original semantic assertions; allocation failure is a test failure. */
typedef struct FixtureStorage {
    ToolTestFixture f;
    UmiAiCodingToolPlan plan;
    UmiAiCodingToolPlanResult result;
} FixtureStorage;

static int CheckFixture(FixtureStorage *state)
{
    UmiAiCodingToolPlanStep step = {0};


    assert(tool_test_fixture_init(&state->f) == UMI_STATUS_OK);
    assert(test_workspace_add(
        &state->f.workspace_storage, "src/a.c", "int a;\n") == UMI_STATUS_OK);

    assert(umi_ai_coding_tool_plan_init(
        &state->plan, "inspect", "Inspect", "Read before reasoning.") == UMI_STATUS_OK);

    (void)strcpy(step.step_id, "exists");
    step.call.call_id = 20U;
    (void)strcpy(step.call.tool_id, "workspace.exists");
    (void)strcpy(step.call.arguments_json, "{\"path\":\"src/a.c\"}");
    step.required = 1;
    assert(umi_ai_coding_tool_plan_add(&state->plan, &step) == UMI_STATUS_OK);

    (void)memset(&step, 0, sizeof(step));
    (void)strcpy(step.step_id, "read");
    step.call.call_id = 21U;
    (void)strcpy(step.call.tool_id, "workspace.read");
    (void)strcpy(step.call.arguments_json, "{\"path\":\"src/a.c\"}");
    step.required = 1;
    step.has_dependency = 1;
    step.depends_on_index = 0U;
    assert(umi_ai_coding_tool_plan_add(&state->plan, &step) == UMI_STATUS_OK);

    assert(umi_ai_coding_tool_plan_execute(
        &state->f.executor, &state->plan, &state->result) == UMI_STATUS_OK);
    assert(state->result.succeeded);
    assert(state->result.passed_count == 2U);

    tool_test_fixture_deinit(&state->f);
    return 0;
}

int main(void)
{
    FixtureStorage *state = calloc(1U, sizeof *state);
    if (state == NULL) {
        fputs("Cannot allocate fixture storage\n", stderr);
        return 1;
    }
    int result = CheckFixture(state);
    free(state);
    return result;
}
