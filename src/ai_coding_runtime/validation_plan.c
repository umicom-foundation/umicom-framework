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

void umi_ai_coding_validation_plan_init(UmiAiCodingValidationPlan *plan)
{
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->revision = 1U;
}

UmiStatus umi_ai_coding_validation_plan_add(
    UmiAiCodingValidationPlan *plan,
    const UmiAiCodingValidationStep *step)
{
    if (plan == NULL || step == NULL ||
        step->id[0] == '\0' ||
        step->executable[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (plan->step_count >= UMI_AI_CODING_RUNTIME_VALIDATION_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    plan->steps[plan->step_count++] = *step;
    plan->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_validation_plan_at(
    const UmiAiCodingValidationPlan *plan,
    size_t index,
    UmiAiCodingValidationStep *out_step)
{
    if (plan == NULL || out_step == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (index >= plan->step_count) return UMI_STATUS_NOT_FOUND;

    *out_step = plan->steps[index];
    return UMI_STATUS_OK;
}
