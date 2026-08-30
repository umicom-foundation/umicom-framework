/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/plan_executor.h
 *
 * PURPOSE:
 *   Execute a bounded tool plan in dependency order and stop or continue
 *   according to each step's explicit failure policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_PLAN_EXECUTOR_H
#define UMICOM_AI_CODING_TOOLS_PLAN_EXECUTOR_H
#include "umicom/ai_coding_tools/executor.h"
#include "umicom/ai_coding_tools/plan.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_coding_tool_plan_execute(
    UmiAiCodingToolExecutor *executor,
    const UmiAiCodingToolPlan *plan,
    UmiAiCodingToolPlanResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
