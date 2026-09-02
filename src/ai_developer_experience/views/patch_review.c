/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/views/patch_review.c
 *
 * PURPOSE:
 *   Project governed patch review progress and changed-file evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/views/patch_review.h"

#include "umicom/base/text.h"

#include <stdio.h>

#include "umicom/ai_developer_experience/action_ids.h"

/*
 * Initialise ai developer patch review view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_ai_developer_patch_review_view_create(
    const char *view_id,
    const UmiAiDeveloperPatchReviewService *review,
    UmiUiViewModel **out_view)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ai_developer_view_create_base(
        view_id,
        "ai-developer-patch-review",
        "AI Patch Review",
        "Complete-file review with per-file hashes, line counts and explicit approval readiness.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_view_set_boolean(
        *out_view, "ai-review.loaded", review->loaded);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!review->loaded || status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_view_set_string(
        *out_view, "ai-review.patch-id", review->review.patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_string(
            *out_view, "ai-review.title", review->review.title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_string(
            *out_view, "ai-review.rationale", review->review.rationale);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-review.file-count",
            (int64_t)review->review.file_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-review.reviewed-file-count",
            (int64_t)review->review.reviewed_file_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-review.added-lines",
            (int64_t)review->review.added_lines);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_integer(
            *out_view,
            "ai-review.removed-lines",
            (int64_t)review->review.removed_lines);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_boolean(
            *out_view,
            "ai-review.ready-to-approve",
            umi_ai_developer_patch_review_service_ready_to_approve(review));

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK && index < review->review.file_count;
         ++index) {
        const UmiAiDeveloperPatchReviewFile *file =
            &review->review.files[index];
        char key[96];
        char row[UMI_UI_VALUE_STRING_CAPACITY];

        (void)snprintf(key, sizeof(key), "ai-review.row.%zu", index);
        (void)umi_text_format(
            row,
            sizeof(row),
            "%s%s | %s | +%u -%u | %s",
            file->selected ? "* " : "",
            file->path,
            umi_ai_coding_patch_operation_text(file->operation),
            file->added_lines,
            file->removed_lines,
            file->reviewed ? "reviewed" : "not reviewed");

        status = umi_ai_developer_view_set_string(*out_view, key, row);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 0U,
            umi_ai_developer_action_open_diff(),
            "Open Diff",
            "Review the selected file line by line",
            review->review.file_count > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 1U,
            umi_ai_developer_action_patch_apply(),
            "Apply",
            "Apply the approved governed patch",
            umi_ai_developer_patch_review_service_ready_to_approve(review));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 2U,
            umi_ai_developer_action_patch_reject(),
            "Reject",
            "Reject the current patch",
            review->loaded);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_ai_developer_view_set_action(
            *out_view, 3U,
            umi_ai_developer_action_open_tasks(),
            "Tasks",
            "Return to AI tasks",
            1);

    return status;
}
