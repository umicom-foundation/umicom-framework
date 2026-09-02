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

/**
 * Represent the ai developer patch review service data shared with callers of this public
 * contract.
 */
typedef struct UmiAiDeveloperPatchReviewService {
    UmiAiCodingPatch patch;
    UmiAiDeveloperPatchReview review;
    UmiAiDeveloperFileDiff diff;
    UmiAiDeveloperSideBySideDiff split;
    uint64_t revision;
    int loaded;
} UmiAiDeveloperPatchReviewService;

/**
 * Initialise ai developer patch review service from caller-provided values so later
 * operations receive a known state.
 */
void umi_ai_developer_patch_review_service_init(
    UmiAiDeveloperPatchReviewService *service);

/**
 * Read ai developer patch review service into validated module state and return a status
 * when input cannot be used.
 */
UmiStatus umi_ai_developer_patch_review_service_load(
    UmiAiDeveloperPatchReviewService *service,
    const UmiAiCodingPatch *patch);

/**
 * Provide the ai developer patch review service select file operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_developer_patch_review_service_select_file(
    UmiAiDeveloperPatchReviewService *service,
    size_t file_index);

/**
 * Provide the ai developer patch review service mark reviewed operation used by this
 * module and its client applications.
 */
UmiStatus umi_ai_developer_patch_review_service_mark_reviewed(
    UmiAiDeveloperPatchReviewService *service,
    size_t file_index,
    int reviewed);

/**
 * Provide the ai developer patch review service refresh diff operation used by this module
 * and its client applications.
 */
UmiStatus umi_ai_developer_patch_review_service_refresh_diff(
    UmiAiDeveloperPatchReviewService *service);

/**
 * Provide the ai developer patch review service ready to approve operation used by this
 * module and its client applications.
 */
int umi_ai_developer_patch_review_service_ready_to_approve(
    const UmiAiDeveloperPatchReviewService *service);

#ifdef __cplusplus
}
#endif
#endif
