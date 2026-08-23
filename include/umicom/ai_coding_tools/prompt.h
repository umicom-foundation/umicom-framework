/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/prompt.h
 *
 * PURPOSE:
 *   Build a provider-readable description of currently permitted coding tools
 *   and the strict textual tool-call/plan protocol used when native provider
 *   tool calling is unavailable.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_PROMPT_H
#define UMICOM_AI_CODING_TOOLS_PROMPT_H
#include "umicom/ai_coding_tools/manifest.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_coding_tool_prompt_build(
    const UmiAiCodingToolEnvironment *environment,
    char *out_text,
    size_t capacity);

const char *umi_ai_coding_tool_call_protocol(void);
const char *umi_ai_coding_tool_plan_protocol(void);

#ifdef __cplusplus
}
#endif
#endif
