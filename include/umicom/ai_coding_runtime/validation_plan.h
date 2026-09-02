/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_runtime/validation_plan.h
 *
 * PURPOSE:
 *   Define an ordered, bounded build/test/lint/package validation plan for an
 *   AI-generated workspace change.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_RUNTIME_VALIDATION_PLAN_H
#define UMICOM_AI_CODING_RUNTIME_VALIDATION_PLAN_H

#include "umicom/ai_coding_runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ai coding validation plan data shared with callers of this public
 * contract.
 */
typedef struct UmiAiCodingValidationPlan {
    UmiAiCodingValidationStep
        steps[UMI_AI_CODING_RUNTIME_VALIDATION_CAPACITY];
    size_t step_count;
    uint64_t revision;
} UmiAiCodingValidationPlan;

/**
 * Initialise ai coding validation plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_ai_coding_validation_plan_init(UmiAiCodingValidationPlan *plan);

/**
 * Add ai coding validation plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ai_coding_validation_plan_add(
    UmiAiCodingValidationPlan *plan,
    const UmiAiCodingValidationStep *step);

/**
 * Find ai coding validation plan while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_ai_coding_validation_plan_at(
    const UmiAiCodingValidationPlan *plan,
    size_t index,
    UmiAiCodingValidationStep *out_step);

#ifdef __cplusplus
}
#endif
#endif
