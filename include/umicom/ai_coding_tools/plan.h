/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/plan.h
 *
 * PURPOSE:
 *   Construct and validate bounded multi-step developer tool plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_PLAN_H
#define UMICOM_AI_CODING_TOOLS_PLAN_H
#include "umicom/ai_coding_tools/policy.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_coding_tool_plan_init(
    UmiAiCodingToolPlan *plan,
    const char *plan_id,
    const char *title,
    const char *rationale);

UmiStatus umi_ai_coding_tool_plan_add(
    UmiAiCodingToolPlan *plan,
    const UmiAiCodingToolPlanStep *step);

UmiStatus umi_ai_coding_tool_plan_validate(
    const UmiAiCodingToolPlan *plan,
    const UmiAiCodingToolPolicy *policy);

#ifdef __cplusplus
}
#endif
#endif
