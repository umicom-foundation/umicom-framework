/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/plan.h
 *
 * PURPOSE:
 *   Construct and validate bounded multi-step developer tool plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_PLAN_H
#define UMICOM_AI_CODING_TOOLS_PLAN_H
#include "umicom/ai_coding_tools/policy.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise ai coding tool plan from caller-provided values so later operations receive a
 * known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_ai_coding_tool_plan_init(
    UmiAiCodingToolPlan *plan,
    const char *plan_id,
    const char *title,
    const char *rationale);

/**
 * Add ai coding tool plan only after its inputs and available capacity have been checked.
 */
UmiStatus umi_ai_coding_tool_plan_add(
    UmiAiCodingToolPlan *plan,
    const UmiAiCodingToolPlanStep *step);

/**
 * Check that ai coding tool plan satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ai_coding_tool_plan_validate(
    const UmiAiCodingToolPlan *plan,
    const UmiAiCodingToolPolicy *policy);

#ifdef __cplusplus
}
#endif
#endif
