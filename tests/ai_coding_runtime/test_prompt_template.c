/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_prompt_template.c
 *
 * PURPOSE:
 *   Verify AI coding runtime prompt template behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/prompt_template.h"

int main(void)
{

    assert(strstr(
        umi_ai_coding_prompt_system(UMI_AI_CODING_TASK_REFACTOR),
        "complete-file") != NULL);
    assert(strstr(
        umi_ai_coding_prompt_response_protocol(),
        "UMICOM-CODING-RESPONSE/1") != NULL);
    assert(umi_ai_coding_prompt_task_may_modify(
        UMI_AI_CODING_TASK_GENERATE_TESTS));
    assert(!umi_ai_coding_prompt_task_may_modify(
        UMI_AI_CODING_TASK_EXPLAIN));

    return 0;
}
