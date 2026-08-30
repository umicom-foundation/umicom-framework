/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_patch_review_view.c
 *
 * PURPOSE:
 *   Toolkit-neutral view coverage for AI Developer Experience patch review view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ai_developer_experience/views/patch_review.h"
#include "umicom/ui/command_view.h"

int main(void)
{
    UmiAiCodingPatch patch;
    UmiAiDeveloperPatchReviewService review;
    UmiUiViewModel *view = NULL;
    UmiUiCommandViewAction apply;

    assert(umi_ai_coding_patch_init(
        &patch, "patch.1", "request.1", "Update", "Reason") == UMI_STATUS_OK);
    assert(umi_ai_coding_patch_add_file(
        &patch, "a.c", UMI_AI_CODING_PATCH_MODIFY,
        "a\n", "b\n") == UMI_STATUS_OK);

    umi_ai_developer_patch_review_service_init(&review);
    assert(umi_ai_developer_patch_review_service_load(
        &review, &patch) == UMI_STATUS_OK);

    assert(umi_ai_developer_patch_review_view_create(
        "test.review", &review, &view) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(view, 1U, &apply) == UMI_STATUS_OK);
    assert(!apply.enabled);

    assert(umi_ai_developer_patch_review_service_mark_reviewed(
        &review, 0U, 1) == UMI_STATUS_OK);
    umi_ui_view_model_destroy(view);
    view = NULL;

    assert(umi_ai_developer_patch_review_view_create(
        "test.review.2", &review, &view) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(view, 1U, &apply) == UMI_STATUS_OK);
    assert(apply.enabled);

    umi_ui_view_model_destroy(view);
    return 0;
}

