/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_diff_navigation.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience diff navigation.
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
#include "umicom/ai_developer_experience/diff_navigation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAiDeveloperFileDiff diff;
    size_t next = 0U;
    size_t previous = 0U;

    assert(umi_ai_developer_diff_build(
        "a.c", UMI_AI_CODING_PATCH_MODIFY,
        "a\nb\nc\n", "a\nB\nc\n",
        1U, 2U, &diff) == UMI_STATUS_OK);

    assert(umi_ai_developer_diff_next_change(
        &diff, 0U, &next) == UMI_STATUS_OK);
    assert(diff.lines[next].kind != UMI_AI_DEVELOPER_DIFF_CONTEXT);

    assert(umi_ai_developer_diff_previous_change(
        &diff, diff.line_count, &previous) == UMI_STATUS_OK);
    assert(diff.lines[previous].kind != UMI_AI_DEVELOPER_DIFF_CONTEXT);
    return 0;
}

