/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/patch_review.h
 *
 * PURPOSE:
 *   Build complete patch-review evidence from the governed UmiAiCodingPatch:
 *   per-file line diffs, hunks, counts and selected review file.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PATCH_REVIEW_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PATCH_REVIEW_H

#include "umicom/ai_developer_experience/diff_builder.h"
#include "umicom/ai_developer_experience/diff_hunks.h"
#include "umicom/ai_developer_experience/side_by_side.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperPatchReviewFile {
    char path[UMI_AI_DEVELOPER_PATH_CAPACITY];
    UmiAiCodingPatchOperation operation;
    uint32_t added_lines;
    uint32_t removed_lines;
    uint64_t before_hash;
    uint64_t after_hash;
    int selected;
    int reviewed;
} UmiAiDeveloperPatchReviewFile;

typedef struct UmiAiDeveloperPatchReview {
    char patch_id[UMI_AI_ID_CAPACITY];
    char request_id[UMI_AI_ID_CAPACITY];
    char title[UMI_AI_DEVELOPER_TITLE_CAPACITY];
    char rationale[UMI_AI_DEVELOPER_TEXT_CAPACITY];
    UmiAiCodingPatchState patch_state;
    UmiAiDeveloperPatchReviewFile
        files[UMI_AI_DEVELOPER_REVIEW_FILE_CAPACITY];
    size_t file_count;
    size_t selected_file_index;
    size_t reviewed_file_count;
    uint32_t added_lines;
    uint32_t removed_lines;
    uint64_t revision;
} UmiAiDeveloperPatchReview;

UmiStatus umi_ai_developer_patch_review_build(
    const UmiAiCodingPatch *patch,
    UmiAiDeveloperPatchReview *out_review);

UmiStatus umi_ai_developer_patch_review_select(
    UmiAiDeveloperPatchReview *review,
    size_t file_index);

UmiStatus umi_ai_developer_patch_review_mark_reviewed(
    UmiAiDeveloperPatchReview *review,
    size_t file_index,
    int reviewed);

UmiStatus umi_ai_developer_patch_review_diff(
    const UmiAiCodingPatch *patch,
    const UmiAiDeveloperPatchReview *review,
    UmiAiDeveloperFileDiff *out_diff);

#ifdef __cplusplus
}
#endif
#endif
