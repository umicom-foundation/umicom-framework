/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_types.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience types.
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
#include "umicom/ai_developer_experience/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    assert(strcmp(
        umi_ai_developer_pane_text(UMI_AI_DEVELOPER_PANE_DIFF),
        "diff") == 0);
    assert(strcmp(
        umi_ai_developer_approval_state_text(
            UMI_AI_DEVELOPER_APPROVAL_PENDING),
        "pending") == 0);
    assert(strcmp(
        umi_ai_developer_task_state_text(
            UMI_AI_DEVELOPER_TASK_VALIDATING),
        "validating") == 0);
    return 0;
}

