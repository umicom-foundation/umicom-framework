/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/patch_review.c
 *
 * PURPOSE:
 *   Implement patch-review projection without changing governed patch state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/patch_review.h"

#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(
    char *out,
    size_t capacity,
    const char *text)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || capacity == 0U || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer patch review build operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_patch_review_build(
    const UmiAiCodingPatch *patch,
    UmiAiDeveloperPatchReview *out_review)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || out_review == NULL ||
        patch->file_count > UMI_AI_DEVELOPER_REVIEW_FILE_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_review, 0, sizeof(*out_review));

    status = copy_text(
        out_review->patch_id,
        sizeof(out_review->patch_id),
        patch->patch_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_review->request_id,
            sizeof(out_review->request_id),
            patch->request_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_review->title,
            sizeof(out_review->title),
            patch->title);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_review->rationale,
            sizeof(out_review->rationale),
            patch->rationale);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_review->patch_state = patch->state;
    out_review->file_count = patch->file_count;
    out_review->revision = patch->revision;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < patch->file_count; ++index) {
        const UmiAiCodingPatchFile *source = &patch->files[index];
        UmiAiDeveloperPatchReviewFile *target = &out_review->files[index];

        status = copy_text(
            target->path,
            sizeof(target->path),
            source->path);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        target->operation = source->operation;
        target->added_lines = source->added_lines;
        target->removed_lines = source->removed_lines;
        target->before_hash = source->before_hash;
        target->after_hash = source->after_hash;
        target->selected = index == 0U;

        out_review->added_lines += source->added_lines;
        out_review->removed_lines += source->removed_lines;
    }

    out_review->selected_file_index = 0U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer patch review select operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_patch_review_select(
    UmiAiDeveloperPatchReview *review,
    size_t file_index)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL || file_index >= review->file_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < review->file_count; ++index) {
        review->files[index].selected = index == file_index;
    }

    review->selected_file_index = file_index;
    review->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer patch review mark reviewed operation used by this module and
 * its client applications.
 */
UmiStatus umi_ai_developer_patch_review_mark_reviewed(
    UmiAiDeveloperPatchReview *review,
    size_t file_index,
    int reviewed)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL || file_index >= review->file_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (review->files[file_index].reviewed != (reviewed != 0)) {
        review->files[file_index].reviewed = reviewed != 0;

        /* Apply this branch only when its contract condition is satisfied. */
        if (reviewed) {
            review->reviewed_file_count += 1U;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (review->reviewed_file_count > 0U) {
            review->reviewed_file_count -= 1U;
        }

        review->revision += 1U;
    }

    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer patch review diff operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_patch_review_diff(
    const UmiAiCodingPatch *patch,
    const UmiAiDeveloperPatchReview *review,
    UmiAiDeveloperFileDiff *out_diff)
{
    const UmiAiCodingPatchFile *file;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (patch == NULL || review == NULL || out_diff == NULL ||
        review->selected_file_index >= patch->file_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    file = &patch->files[review->selected_file_index];

    status = umi_ai_developer_diff_build(
        file->path,
        file->operation,
        file->before_text,
        file->after_text,
        file->before_hash,
        file->after_hash,
        out_diff);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    return umi_ai_developer_diff_build_hunks(out_diff, 3U);
}
