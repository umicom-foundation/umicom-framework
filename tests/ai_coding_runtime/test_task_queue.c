/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_coding_runtime/test_task_queue.c
 *
 * PURPOSE:
 *   Verify AI coding runtime task queue behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/ai_coding_runtime/task_queue.h"

int main(void)
{

    UmiAiCodingTaskQueue *queue = NULL;
    UmiAiCodingQueuedTask task = {0};
    UmiAiCodingQueuedTask output;
    (void)strcpy(task.task_id, "task");
    umi_ai_coding_request_init(&task.request, UMI_AI_CODING_TASK_CHAT);
    assert(umi_ai_coding_task_queue_create(&queue) == UMI_STATUS_OK);
    assert(umi_ai_coding_task_queue_push(queue, &task) == UMI_STATUS_OK);
    assert(umi_ai_coding_task_queue_peek(queue, &output) == UMI_STATUS_OK);
    assert(strcmp(output.task_id, "task") == 0);
    assert(umi_ai_coding_task_queue_pop(queue, &output) == UMI_STATUS_OK);
    assert(umi_ai_coding_task_queue_count(queue) == 0U);
    umi_ai_coding_task_queue_destroy(queue);

    return 0;
}
