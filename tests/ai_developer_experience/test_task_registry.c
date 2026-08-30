/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_task_registry.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience task registry.
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
#include "umicom/ai_developer_experience/task_registry.h"

int main(void)
{
    UmiAiDeveloperTaskRegistry *registry = NULL;
    UmiAiDeveloperTaskEntry entry = {0};
    UmiAiDeveloperTaskEntry copy;

    assert(umi_ai_developer_task_registry_create(&registry) == UMI_STATUS_OK);
    (void)strcpy(entry.task_id, "task.1");
    (void)strcpy(entry.title, "Task");
    entry.state = UMI_AI_DEVELOPER_TASK_RUNNING;

    assert(umi_ai_developer_task_registry_upsert(
        registry, &entry) == UMI_STATUS_OK);
    assert(umi_ai_developer_task_registry_count(registry) == 1U);

    entry.state = UMI_AI_DEVELOPER_TASK_COMPLETED;
    assert(umi_ai_developer_task_registry_upsert(
        registry, &entry) == UMI_STATUS_OK);
    assert(umi_ai_developer_task_registry_count(registry) == 1U);
    assert(umi_ai_developer_task_registry_find(
        registry, "task.1", &copy) == UMI_STATUS_OK);
    assert(copy.state == UMI_AI_DEVELOPER_TASK_COMPLETED);

    umi_ai_developer_task_registry_destroy(registry);
    return 0;
}

