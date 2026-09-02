/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_tools/test_plan_parser.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI coding tools plan parser.
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
#include "umicom/ai_coding_tools/plan_parser.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiCodingToolPlan plan;

    assert(umi_ai_coding_tool_plan_parse(
        "UMICOM-TOOL-PLAN/1\n"
        "PLAN|inspect|Inspect workspace\n"
        "RATIONALE-BEGIN\nRead before reasoning.\nRATIONALE-END\n"
        "STEP-BEGIN|one|workspace.exists|1|0\n"
        "ARGUMENTS-BEGIN\n{\"path\":\"src/main.c\"}\nARGUMENTS-END\n"
        "STEP-END\n"
        "STEP-BEGIN|two|workspace.read|1|0\n"
        "ARGUMENTS-BEGIN\n{\"path\":\"src/main.c\"}\nARGUMENTS-END\n"
        "STEP-END\n"
        "PLAN-END\n",
        100U,
        &plan) == UMI_STATUS_OK);

    assert(plan.step_count == 2U);
    assert(strcmp(plan.plan_id, "inspect") == 0);
    assert(plan.steps[1].has_dependency);
    assert(plan.steps[1].depends_on_index == 0U);
    assert(plan.steps[0].call.call_id == 100U);
    assert(plan.steps[1].call.call_id == 101U);
    return 0;
}

