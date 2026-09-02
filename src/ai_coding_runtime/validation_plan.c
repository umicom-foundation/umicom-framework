/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/validation_plan.c
 *
 * PURPOSE:
 *   Implement ordered validation-plan storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/validation_plan.h"

#include <string.h>

/*
 * Initialise ai coding validation plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_validation_plan_init(UmiAiCodingValidationPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->revision = 1U;
}

/*
 * Add ai coding validation plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_coding_validation_plan_add(
    UmiAiCodingValidationPlan *plan,
    const UmiAiCodingValidationStep *step)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || step == NULL ||
        step->id[0] == '\0' ||
        step->executable[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->step_count >= UMI_AI_CODING_RUNTIME_VALIDATION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    plan->steps[plan->step_count++] = *step;
    plan->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find ai coding validation plan while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_coding_validation_plan_at(
    const UmiAiCodingValidationPlan *plan,
    size_t index,
    UmiAiCodingValidationStep *out_step)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || out_step == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= plan->step_count) return UMI_STATUS_NOT_FOUND;

    *out_step = plan->steps[index];
    return UMI_STATUS_OK;
}
