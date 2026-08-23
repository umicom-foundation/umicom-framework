/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_developer_experience/test_task_projection.c
 *
 * PURPOSE:
 *   Focused regression coverage for AI Developer Experience task projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/ai_developer_experience/task_projection.h"

int main(void)
{
    UmiAiCodingAgentSnapshot snapshot = {0};
    UmiAiDeveloperTaskEntry entry;

    (void)strcpy(snapshot.task_id, "task.1");
    (void)strcpy(snapshot.request_id, "request.1");
    snapshot.state = UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED;
    snapshot.iteration = 2U;
    snapshot.maximum_iterations = 3U;
    snapshot.patch_files = 4U;
    snapshot.validation_failures = 1U;

    assert(umi_ai_developer_task_project(
        &snapshot, "Task", "Summary", 10U, &entry) == UMI_STATUS_OK);
    assert(entry.state == UMI_AI_DEVELOPER_TASK_REVIEW);
    assert(entry.iteration == 2U);
    assert(entry.patch_files == 4U);
    return 0;
}

