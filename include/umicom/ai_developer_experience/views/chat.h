/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/chat.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Coding Chat view model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_CHAT_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_CHAT_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai_coding_tools/chat_session.h"
/**
 * Initialise ai developer chat view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_ai_developer_chat_view_create(
    const char *view_id,
    const UmiAiCodingToolChatSession *session,
    size_t visible_rows,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
