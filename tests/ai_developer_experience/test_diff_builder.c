/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_diff_builder.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience diff builder.
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

int main(void)
{
    UmiAiDeveloperFileDiff diff;

    assert(umi_ai_developer_diff_build(
        "src/main.c",
        UMI_AI_CODING_PATCH_MODIFY,
        "one\ntwo\nthree\n",
        "one\nTWO\nthree\nfour\n",
        1U, 2U, &diff) == UMI_STATUS_OK);

    assert(diff.added_count == 2U);
    assert(diff.removed_count == 1U);
    assert(diff.context_count == 2U);
    assert(diff.line_count == 5U);
    return 0;
}

