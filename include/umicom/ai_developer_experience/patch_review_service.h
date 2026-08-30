/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/patch_review_service.h
 *
 * PURPOSE:
 *   Own the currently reviewed governed patch and its line/split diff projections
 *   without moving patch approval or apply authority into presentation code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_PATCH_REVIEW_SERVICE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_PATCH_REVIEW_SERVICE_H
#include "umicom/ai_developer_experience/patch_review.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiDeveloperPatchReviewService {
    UmiAiCodingPatch patch;
    UmiAiDeveloperPatchReview review;
    UmiAiDeveloperFileDiff diff;
    UmiAiDeveloperSideBySideDiff split;
    uint64_t revision;
    int loaded;
} UmiAiDeveloperPatchReviewService;

void umi_ai_developer_patch_review_service_init(
    UmiAiDeveloperPatchReviewService *service);

UmiStatus umi_ai_developer_patch_review_service_load(
    UmiAiDeveloperPatchReviewService *service,
    const UmiAiCodingPatch *patch);

UmiStatus umi_ai_developer_patch_review_service_select_file(
    UmiAiDeveloperPatchReviewService *service,
    size_t file_index);

UmiStatus umi_ai_developer_patch_review_service_mark_reviewed(
    UmiAiDeveloperPatchReviewService *service,
    size_t file_index,
    int reviewed);

UmiStatus umi_ai_developer_patch_review_service_refresh_diff(
    UmiAiDeveloperPatchReviewService *service);

int umi_ai_developer_patch_review_service_ready_to_approve(
    const UmiAiDeveloperPatchReviewService *service);

#ifdef __cplusplus
}
#endif
#endif
