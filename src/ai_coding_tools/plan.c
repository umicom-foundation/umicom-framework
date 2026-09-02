/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/plan.c
 *
 * PURPOSE:
 *   Implement bounded multi-step developer tool planning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/plan.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *value)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || capacity == 0U || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise ai coding tool plan from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ai_coding_tool_plan_init(
    UmiAiCodingToolPlan *plan,
    const char *plan_id,
    const char *title,
    const char *rationale)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(plan, 0, sizeof(*plan));

    status = copy_text(plan->plan_id, sizeof(plan->plan_id), plan_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(plan->title, sizeof(plan->title), title);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            plan->rationale,
            sizeof(plan->rationale),
            rationale);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    plan->revision = 1U;
    return UMI_STATUS_OK;
}

/* Add ai coding tool plan only after its inputs and available capacity have been checked. */
UmiStatus umi_ai_coding_tool_plan_add(
    UmiAiCodingToolPlan *plan,
    const UmiAiCodingToolPlanStep *step)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || step == NULL ||
        step->step_id[0] == '\0' ||
        step->call.tool_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->step_count >= UMI_AI_CODING_TOOL_PLAN_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (step->has_dependency &&
        step->depends_on_index >= plan->step_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->step_count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(
                plan->steps[index].step_id,
                step->step_id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    plan->steps[plan->step_count++] = *step;
    plan->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Check that ai coding tool plan satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ai_coding_tool_plan_validate(
    const UmiAiCodingToolPlan *plan,
    const UmiAiCodingToolPolicy *policy)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || policy == NULL ||
        plan->plan_id[0] == '\0' ||
        plan->step_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->step_count > policy->maximum_plan_steps) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < plan->step_count; ++index) {
        const UmiAiCodingToolPlanStep *step = &plan->steps[index];

        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (step->step_id[0] == '\0' ||
            step->call.tool_id[0] == '\0' ||
            (step->has_dependency &&
             step->depends_on_index >= index)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }

    return UMI_STATUS_OK;
}
