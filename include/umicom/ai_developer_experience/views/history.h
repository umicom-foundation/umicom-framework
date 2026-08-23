/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/history.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI History view model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_HISTORY_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_HISTORY_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai_developer_experience/timeline.h"
UmiStatus umi_ai_developer_history_view_create(
    const char *view_id,
    const UmiAiDeveloperTimeline *timeline,
    size_t visible_rows,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
