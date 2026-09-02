/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_ai_prompt_context.c
 *
 * PURPOSE:
 *   Validate the AI and Helix foundation introduced by Batch 10.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused executable keeps one contract easy to diagnose when the larger test suite reports a failure.
 */



#include <assert.h>
#include <string.h>
#include "umicom/umicom.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiPrompt prompt;
    UmiAiTokenBudget budget;
    UmiAiContext context;
    umi_ai_prompt_init(&prompt);
    assert(umi_ai_prompt_append_line(&prompt, "system") == UMI_STATUS_OK);
    assert(umi_ai_prompt_append(&prompt, "hello") == UMI_STATUS_OK);
    assert(strstr(prompt.text, "hello") != NULL);
    assert(umi_ai_token_budget_set(&budget, 4096U, 512U, prompt.text) == UMI_STATUS_OK);
    assert(umi_ai_context_configure(&context, 4096U, 512U) == UMI_STATUS_OK);
    assert(umi_ai_context_consume(&context, budget.estimated_input) == UMI_STATUS_OK);
    assert(umi_ai_context_remaining(&context) > 0U);
    return 0;
}
