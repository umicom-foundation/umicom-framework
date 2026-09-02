/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/task_controller.h
 *
 * PURPOSE:
 *   Provide reusable task-level controls over the operational AI coding agent.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_CONTROLLER_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_CONTROLLER_H
#include "umicom/ai_developer_experience/platform.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai developer task cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_task_cancel(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer task continue repair operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_task_continue_repair(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer task apply pending operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_task_apply_pending(
    UmiAiDeveloperExperiencePlatform *platform);

/**
 * Provide the ai developer task reject pending operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_task_reject_pending(
    UmiAiDeveloperExperiencePlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
