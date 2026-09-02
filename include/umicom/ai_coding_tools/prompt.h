/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_coding_tools/prompt.h
 *
 * PURPOSE:
 *   Build a provider-readable description of currently permitted coding tools
 *   and the strict textual tool-call/plan protocol used when native provider
 *   tool calling is unavailable.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_CODING_TOOLS_PROMPT_H
#define UMICOM_AI_CODING_TOOLS_PROMPT_H
#include "umicom/ai_coding_tools/manifest.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai coding tool prompt build operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_coding_tool_prompt_build(
    const UmiAiCodingToolEnvironment *environment,
    char *out_text,
    size_t capacity);

/**
 * Provide the ai coding tool call protocol operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_tool_call_protocol(void);
/**
 * Provide the ai coding tool plan protocol operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_tool_plan_protocol(void);

#ifdef __cplusplus
}
#endif
#endif
