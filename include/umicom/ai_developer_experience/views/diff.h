/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/diff.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Diff Review view model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_DIFF_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_DIFF_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai_developer_experience/patch_review_service.h"
#include "umicom/ai_developer_experience/preferences.h"
UmiStatus umi_ai_developer_diff_view_create(
    const char *view_id,
    const UmiAiDeveloperPatchReviewService *review,
    const UmiAiDeveloperPreferences *preferences,
    size_t first_row,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
