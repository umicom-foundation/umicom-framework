/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/patch_review.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Patch Review view model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_PATCH_REVIEW_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_PATCH_REVIEW_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai_developer_experience/patch_review_service.h"
/**
 * Initialise ai developer patch review view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_ai_developer_patch_review_view_create(
    const char *view_id,
    const UmiAiDeveloperPatchReviewService *review,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
