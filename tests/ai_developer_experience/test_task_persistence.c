/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_task_persistence.c
 *
 * PURPOSE:
 *   Focused durable-state coverage for AI Developer Experience task persistence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/task_persistence.h"

int main(void)
{
    UmiSessionStore *store = NULL;
    UmiAiDeveloperTaskRegistry *source = NULL;
    UmiAiDeveloperTaskRegistry *restored = NULL;
    UmiAiDeveloperTaskEntry task = {0};
    UmiAiDeveloperTaskEntry copy;
    size_t restored_count = 0U;

    assert(umi_session_store_create(&store) == UMI_STATUS_OK);
    assert(umi_ai_developer_task_registry_create(&source) == UMI_STATUS_OK);
    assert(umi_ai_developer_task_registry_create(&restored) == UMI_STATUS_OK);

    (void)strcpy(task.task_id, "task.1");
    (void)strcpy(task.request_id, "request.1");
    (void)strcpy(task.title, "Build fix");
    (void)strcpy(task.summary, "line one\nline two");
    task.state = UMI_AI_DEVELOPER_TASK_REVIEW;
    task.iteration = 2U;
    task.maximum_iterations = 3U;
    task.patch_files = 4U;

    assert(umi_ai_developer_task_registry_upsert(
        source, &task) == UMI_STATUS_OK);
    assert(umi_ai_developer_tasks_save(
        store, "tasks", source) == UMI_STATUS_OK);
    assert(umi_ai_developer_tasks_restore(
        store, "tasks", restored, &restored_count) == UMI_STATUS_OK);
    assert(restored_count == 1U);
    assert(umi_ai_developer_task_registry_find(
        restored, "task.1", &copy) == UMI_STATUS_OK);
    assert(strcmp(copy.summary, task.summary) == 0);
    assert(copy.iteration == 2U);

    umi_ai_developer_task_registry_destroy(restored);
    umi_ai_developer_task_registry_destroy(source);
    umi_session_store_destroy(store);
    return 0;
}

