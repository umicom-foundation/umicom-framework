/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/task_projection.h
 *
 * PURPOSE:
 *   Project an operational coding-agent snapshot into a compact durable task
 *   summary for presentation and restoration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_PROJECTION_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_TASK_PROJECTION_H
#include "umicom/ai_developer_experience/task_registry.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the ai developer task state from agent operation used by this module and its
 * client applications.
 */
UmiAiDeveloperTaskState umi_ai_developer_task_state_from_agent(
    UmiAiCodingRuntimeState state);

/**
 * Provide the ai developer task project operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_task_project(
    const UmiAiCodingAgentSnapshot *snapshot,
    const char *title,
    const char *summary,
    uint64_t sequence,
    UmiAiDeveloperTaskEntry *out_entry);

#ifdef __cplusplus
}
#endif
#endif
