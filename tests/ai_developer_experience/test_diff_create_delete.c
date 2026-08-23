/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_diff_create_delete.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience diff create delete.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_developer_experience/diff_builder.h"

int main(void)
{
    UmiAiDeveloperFileDiff create_diff;
    UmiAiDeveloperFileDiff delete_diff;

    assert(umi_ai_developer_diff_build(
        "new.c", UMI_AI_CODING_PATCH_CREATE,
        "", "a\nb\n", 0U, 1U, &create_diff) == UMI_STATUS_OK);
    assert(create_diff.added_count == 2U);
    assert(create_diff.removed_count == 0U);

    assert(umi_ai_developer_diff_build(
        "old.c", UMI_AI_CODING_PATCH_DELETE,
        "a\nb\n", "", 1U, 0U, &delete_diff) == UMI_STATUS_OK);
    assert(delete_diff.added_count == 0U);
    assert(delete_diff.removed_count == 2U);
    return 0;
}

