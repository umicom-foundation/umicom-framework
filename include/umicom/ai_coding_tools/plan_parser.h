/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/plan_parser.h
 *
 * PURPOSE:
 *   Parse the strict UMICOM-TOOL-PLAN/1 provider protocol into a bounded
 *   sequential Framework tool plan.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_PLAN_PARSER_H
#define UMICOM_AI_CODING_TOOLS_PLAN_PARSER_H
#include "umicom/ai_coding_tools/plan.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_coding_tool_plan_parse(
    const char *text,
    uint64_t first_call_id,
    UmiAiCodingToolPlan *out_plan);

#ifdef __cplusplus
}
#endif
#endif
