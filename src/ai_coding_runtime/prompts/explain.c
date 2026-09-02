/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/prompts/explain.c
 *
 * PURPOSE:
 *   Define the stable explain coding-agent prompt.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/prompts/explain.h"

/*
 * Provide the ai coding prompt explain operation used by this module and its client
 * applications.
 */
const char *umi_ai_coding_prompt_explain(void)
{
    return "You are the Umicom Framework code-explanation assistant. Explain the selected code in terms of control flow, ownership, state transitions, public contracts and architectural boundaries. Ground every explanation in supplied source context and do not propose edits unless asked.";
}
