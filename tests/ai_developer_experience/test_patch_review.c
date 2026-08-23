/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_patch_review.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience patch review.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/patch_review.h"

int main(void)
{
    UmiAiCodingPatch patch;
    UmiAiDeveloperPatchReview review;
    UmiAiDeveloperFileDiff diff;

    assert(umi_ai_coding_patch_init(
        &patch, "patch.1", "request.1", "Update", "Reason") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, "src/a.c", UMI_AI_CODING_PATCH_MODIFY,
        "int a = 1;\n", "int a = 2;\n") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, "src/b.c", UMI_AI_CODING_PATCH_CREATE,
        "", "int b = 1;\n") == UMI_STATUS_OK);

    assert(umi_ai_developer_patch_review_build(
        &patch, &review) == UMI_STATUS_OK);
    assert(review.file_count == 2U);
    assert(review.files[0].selected);
    assert(review.reviewed_file_count == 0U);

    assert(umi_ai_developer_patch_review_select(
        &review, 1U) == UMI_STATUS_OK);
    assert(review.files[1].selected);

    assert(umi_ai_developer_patch_review_diff(
        &patch, &review, &diff) == UMI_STATUS_OK);
    assert(strcmp(diff.path, "src/b.c") == 0);
    return 0;
}

