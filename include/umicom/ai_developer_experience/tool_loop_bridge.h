/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/tool_loop_bridge.h
 *
 * PURPOSE:
 *   Configure the controlled Coding Tools agent loop to use the AI Developer
 *   approval centre rather than an application-specific approval callback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_TOOL_LOOP_BRIDGE_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_TOOL_LOOP_BRIDGE_H

#include "umicom/ai_developer_experience/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_developer_tool_loop_configure(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiCodingToolLoopConfig *config);

#ifdef __cplusplus
}
#endif
#endif
