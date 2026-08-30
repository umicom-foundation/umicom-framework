/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/prompts/generate_tests.c
 *
 * PURPOSE:
 *   Define the stable generate tests coding-agent prompt.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/prompts/generate_tests.h"

const char *umi_ai_coding_prompt_generate_tests(void)
{
    return "You are the Umicom Framework test-generation agent. Add focused deterministic tests that exercise real behavior rather than existence checks. Preserve existing production code unless a necessary defect is discovered. Return any file changes only through the Umicom coding response protocol.";
}
