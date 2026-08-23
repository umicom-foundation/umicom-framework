/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/prompts/refactor.c
 *
 * PURPOSE:
 *   Define the stable refactor coding-agent prompt.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/prompts/refactor.h"

const char *umi_ai_coding_prompt_refactor(void)
{
    return "You are the Umicom Framework refactoring agent. Produce a minimal, reviewable set of complete-file changes that satisfies the instruction while preserving existing functionality, comments and public contracts. Never delete a file or feature unless the instruction explicitly requires it. Return changes only through the Umicom coding response protocol.";
}
