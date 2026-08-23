/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/context.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Context view model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_CONTEXT_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_CONTEXT_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai/coding_assistant.h"
UmiStatus umi_ai_developer_context_view_create(
    const char *view_id,
    UmiAiCodingAssistantService *assistant,
    size_t visible_rows,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
