/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/overview.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Developer view model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_OVERVIEW_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_OVERVIEW_H
#include "umicom/ai_developer_experience/view_helpers.h"
#include "umicom/ai_developer_experience/task_registry.h"
#include "umicom/ai_developer_experience/approval_queue.h"
#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ai_developer_overview_view_create(
    const char *view_id,
    const UmiAiCodingRuntimePlatformSnapshot *coding,
    const UmiAiCodingToolsPlatformSnapshot *tools,
    const UmiAiDeveloperTaskRegistry *tasks,
    const UmiAiDeveloperApprovalQueue *approvals,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
