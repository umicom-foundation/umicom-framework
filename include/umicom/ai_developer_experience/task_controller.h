/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/task_controller.h
 *
 * PURPOSE:
 *   Provide reusable task-level controls over the operational AI coding agent.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_CONTROLLER_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_CONTROLLER_H
#include "umicom/ai_developer_experience/platform.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_developer_task_cancel(
    UmiAiDeveloperExperiencePlatform *platform);

UmiStatus umi_ai_developer_task_continue_repair(
    UmiAiDeveloperExperiencePlatform *platform);

UmiStatus umi_ai_developer_task_apply_pending(
    UmiAiDeveloperExperiencePlatform *platform);

UmiStatus umi_ai_developer_task_reject_pending(
    UmiAiDeveloperExperiencePlatform *platform);

#ifdef __cplusplus
}
#endif
#endif
