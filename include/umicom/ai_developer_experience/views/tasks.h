/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai_developer_experience/views/tasks.h
 *
 * PURPOSE:
 *   Create the toolkit-neutral AI Tasks view model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_TASKS_H
#define UMICOM_AI_DEVELOPER_EXPERIENCE_VIEW_TASKS_H
#include "umicom/ai_developer_experience/view_helpers.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ai_developer_experience/task_registry.h"
UmiStatus umi_ai_developer_tasks_view_create(
    const char *view_id,
    const UmiAiDeveloperTaskRegistry *tasks,
    const char *active_task_id,
    size_t visible_rows,
    UmiUiViewModel **out_view);
#ifdef __cplusplus
}
#endif
#endif
