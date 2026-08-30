/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_tasks_view.c
 *
 * PURPOSE:
 *   Toolkit-neutral view coverage for AI Developer Experience tasks view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include "umicom/ai_developer_experience/views/tasks.h"
#include "umicom/ui/view_model.h"

int main(void)
{
    UmiAiDeveloperTaskRegistry *tasks = NULL;
    UmiAiDeveloperTaskEntry task = {0};
    UmiUiViewModel *view = NULL;
    UmiUiViewSnapshot snapshot;

    assert(umi_ai_developer_task_registry_create(&tasks) == UMI_STATUS_OK);
    (void)snprintf(task.task_id, sizeof(task.task_id), "%s", "task.1");
    (void)snprintf(task.title, sizeof(task.title), "%s", "Task");
    task.state = UMI_AI_DEVELOPER_TASK_RUNNING;
    assert(umi_ai_developer_task_registry_upsert(tasks, &task) == UMI_STATUS_OK);

    assert(umi_ai_developer_tasks_view_create(
        "test.tasks", tasks, "task.1", 10U, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_snapshot(view, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.visible);
    umi_ui_view_model_destroy(view);
    umi_ai_developer_task_registry_destroy(tasks);
    return 0;
}

