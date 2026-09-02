/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/command_router.h
 *
 * PURPOSE:
 *   Execute stable AI developer commands against the composed Framework
 *   experience platform.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_COMMAND_ROUTER_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_COMMAND_ROUTER_H

#include "umicom/ai_developer_experience/command.h"
#include "umicom/ai_developer_experience/command_context.h"
#include "umicom/ai_developer_experience/diff_navigation.h"
#include "umicom/ai_developer_experience/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai developer command enabled operation used by this module and its client
 * applications.
 */
int umi_ai_developer_command_enabled(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *command_id,
    const UmiAiDeveloperCommandContext *context);

/**
 * Perform ai developer command through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ai_developer_command_execute(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *command_id,
    const UmiAiDeveloperCommandContext *context,
    char *out_message,
    size_t message_capacity);

#ifdef __cplusplus
}
#endif
#endif
