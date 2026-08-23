/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_tools/plan.c
 *
 * PURPOSE:
 *   Implement bounded multi-step developer tool planning.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_tools/plan.h"

#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *value)
{
    size_t length;

    if (out == NULL || capacity == 0U || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(value);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, value, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_plan_init(
    UmiAiCodingToolPlan *plan,
    const char *plan_id,
    const char *title,
    const char *rationale)
{
    UmiStatus status;

    if (plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(plan, 0, sizeof(*plan));

    status = copy_text(plan->plan_id, sizeof(plan->plan_id), plan_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(plan->title, sizeof(plan->title), title);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            plan->rationale,
            sizeof(plan->rationale),
            rationale);
    }
    if (status != UMI_STATUS_OK) return status;

    plan->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_tool_plan_add(
    UmiAiCodingToolPlan *plan,
    const UmiAiCodingToolPlanStep *step)
{
    size_t index;

    if (plan == NULL || step == NULL ||
        step->step_id[0] == '\0' ||
        step->call.tool_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (plan->step_count >= UMI_AI_CODING_TOOL_PLAN_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    if (step->has_dependency &&
        step->depends_on_index >= plan->step_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < plan->step_count; ++index) {
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

UmiStatus umi_ai_coding_tool_plan_validate(
    const UmiAiCodingToolPlan *plan,
    const UmiAiCodingToolPolicy *policy)
{
    size_t index;

    if (plan == NULL || policy == NULL ||
        plan->plan_id[0] == '\0' ||
        plan->step_count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (plan->step_count > policy->maximum_plan_steps) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    for (index = 0U; index < plan->step_count; ++index) {
        const UmiAiCodingToolPlanStep *step = &plan->steps[index];

        if (step->step_id[0] == '\0' ||
            step->call.tool_id[0] == '\0' ||
            (step->has_dependency &&
             step->depends_on_index >= index)) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
    }

    return UMI_STATUS_OK;
}
