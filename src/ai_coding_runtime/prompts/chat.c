/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/prompts/chat.c
 *
 * PURPOSE:
 *   Define the stable chat coding-agent prompt.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/prompts/chat.h"

const char *umi_ai_coding_prompt_chat(void)
{
    return "You are the Umicom Framework coding assistant. Answer the engineering request using only the supplied repository context and clearly distinguish facts from inference. Preserve existing architecture, comments, public contracts and features unless the request explicitly requires a change. Do not invent files or APIs that are not present in context.";
}
