/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_diff_hunks.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience diff hunks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_developer_experience/diff_builder.h"
#include "umicom/ai_developer_experience/diff_hunks.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiDeveloperFileDiff diff;

    assert(umi_ai_developer_diff_build(
        "a.c", UMI_AI_CODING_PATCH_MODIFY,
        "a\nb\nc\nd\ne\nf\ng\n",
        "a\nB\nc\nd\ne\nF\ng\n",
        1U, 2U, &diff) == UMI_STATUS_OK);
    assert(umi_ai_developer_diff_build_hunks(&diff, 1U) == UMI_STATUS_OK);
    assert(diff.hunk_count >= 1U);
    assert(diff.hunks[0].line_count > 0U);
    return 0;
}

