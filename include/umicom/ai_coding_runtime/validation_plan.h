/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/validation_plan.h
 *
 * PURPOSE:
 *   Define an ordered, bounded build/test/lint/package validation plan for an
 *   AI-generated workspace change.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_VALIDATION_PLAN_H
#define UMICOM_AI_CODING_RUNTIME_VALIDATION_PLAN_H

#include "umicom/ai_coding_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiCodingValidationPlan {
    UmiAiCodingValidationStep
        steps[UMI_AI_CODING_RUNTIME_VALIDATION_CAPACITY];
    size_t step_count;
    uint64_t revision;
} UmiAiCodingValidationPlan;

void umi_ai_coding_validation_plan_init(UmiAiCodingValidationPlan *plan);

UmiStatus umi_ai_coding_validation_plan_add(
    UmiAiCodingValidationPlan *plan,
    const UmiAiCodingValidationStep *step);

UmiStatus umi_ai_coding_validation_plan_at(
    const UmiAiCodingValidationPlan *plan,
    size_t index,
    UmiAiCodingValidationStep *out_step);

#ifdef __cplusplus
}
#endif
#endif
