/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/chat_controller.h
 *
 * PURPOSE:
 *   Provide a high-level repository-aware chat workflow that automatically
 *   connects controlled tool calls to the AI Developer approval centre.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_CHAT_CONTROLLER_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_CHAT_CONTROLLER_H

#include "umicom/ai_developer_experience/tool_loop_bridge.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_developer_chat_open(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *session_id,
    const char *provider_id,
    const char *model_id);

UmiStatus umi_ai_developer_chat_run(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *session_id,
    const char *user_message,
    UmiAiCodingToolLoopResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
