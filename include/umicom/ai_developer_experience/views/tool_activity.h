/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/tool_activity.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Tool Activity view model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_TOOL_ACTIVITY_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_TOOL_ACTIVITY_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai_coding_tools/result_history.h"
/**
 * Initialise ai developer tool activity view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_ai_developer_tool_activity_view_create(
    const char *view_id,
    const UmiAiCodingToolResultHistory *history,
    size_t visible_rows,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
