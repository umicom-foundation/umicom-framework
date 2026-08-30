/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/prompts/complete.c
 *
 * PURPOSE:
 *   Define the stable complete coding-agent prompt.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/prompts/complete.h"

const char *umi_ai_coding_prompt_complete(void)
{
    return "You are the Umicom Framework code-completion assistant. Continue the active source coherently with its existing language, naming, ownership and error-handling conventions. Prefer the smallest correct completion. Do not rewrite unrelated code.";
}
