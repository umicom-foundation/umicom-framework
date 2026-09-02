/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/plan_executor.c
 *
 * PURPOSE:
 *   Implement sequential dependency-aware coding tool plan execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/plan_executor.h"

#include <string.h>

/*
 * Perform ai coding tool plan through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ai_coding_tool_plan_execute(
    UmiAiCodingToolExecutor *executor,
    const UmiAiCodingToolPlan *plan,
    UmiAiCodingToolPlanResult *out_result)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (executor == NULL || plan == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_ai_coding_tool_plan_validate(
        plan,
        &executor->environment->policy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(out_result, 0, sizeof(*out_result));
    (void)strcpy(out_result->plan_id, plan->plan_id);
    out_result->revision = plan->revision;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->step_count; ++index) {
        const UmiAiCodingToolPlanStep *step = &plan->steps[index];
        UmiAiCodingToolResult *result =
            &out_result->results[out_result->result_count];

        /* Apply this branch only when its contract condition is satisfied. */
        if (step->has_dependency) {
            const UmiAiCodingToolResult *dependency =
                &out_result->results[step->depends_on_index];

            /* Apply this branch only when its contract condition is satisfied. */
            if (dependency->state !=
                UMI_AI_CODING_TOOL_CALL_SUCCEEDED) {
                result->call_id = step->call.call_id;
                (void)strcpy(result->tool_id, step->call.tool_id);
                result->status = UMI_STATUS_INVALID_STATE;
                result->state = UMI_AI_CODING_TOOL_CALL_CANCELLED;
                (void)strcpy(
                    result->output,
                    "{\"ok\":false,\"message\":\"dependency did not succeed\"}");
                result->output_length = strlen(result->output);
                out_result->result_count += 1U;
                out_result->failed_count += 1U;

                /* Apply this branch only when its contract condition is satisfied. */
                if (step->required && !step->continue_on_failure) {
                    break;
                }
                continue;
            }
        }

        status = umi_ai_coding_tool_execute(
            executor,
            &step->call,
            result);

        out_result->result_count += 1U;

        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result->state == UMI_AI_CODING_TOOL_CALL_SUCCEEDED) {
            out_result->passed_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            out_result->failed_count += 1U;

            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (result->state ==
                UMI_AI_CODING_TOOL_CALL_APPROVAL_REQUIRED ||
                result->state ==
                UMI_AI_CODING_TOOL_CALL_REJECTED) {
                out_result->rejected_count += 1U;
            }

            /* Apply this branch only when its contract condition is satisfied. */
            if (step->required && !step->continue_on_failure) {
                break;
            }
        }
    }

    out_result->completed =
        out_result->result_count == plan->step_count;
    out_result->succeeded =
        out_result->completed &&
        out_result->failed_count == 0U;

    return out_result->succeeded
        ? UMI_STATUS_OK
        : UMI_STATUS_UNAVAILABLE;
}
