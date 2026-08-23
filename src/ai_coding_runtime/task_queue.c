/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/task_queue.c
 *
 * PURPOSE:
 *   Implement heap-owned FIFO coding-task storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/task_queue.h"

#include <stdlib.h>
#include <string.h>

struct UmiAiCodingTaskQueue {
    UmiAiCodingQueuedTask items[UMI_AI_CODING_TASK_QUEUE_CAPACITY];
    size_t head;
    size_t count;
    uint64_t sequence;
};

UmiStatus umi_ai_coding_task_queue_create(UmiAiCodingTaskQueue **out_queue)
{
    UmiAiCodingTaskQueue *queue;

    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;

    queue = (UmiAiCodingTaskQueue *)calloc(1U, sizeof(*queue));
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_queue = queue;
    return UMI_STATUS_OK;
}

void umi_ai_coding_task_queue_destroy(UmiAiCodingTaskQueue *queue)
{
    free(queue);
}

UmiStatus umi_ai_coding_task_queue_push(
    UmiAiCodingTaskQueue *queue,
    const UmiAiCodingQueuedTask *task)
{
    size_t index;

    if (queue == NULL || task == NULL ||
        task->task_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (queue->count >= UMI_AI_CODING_TASK_QUEUE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    index = (queue->head + queue->count) % UMI_AI_CODING_TASK_QUEUE_CAPACITY;
    queue->items[index] = *task;
    queue->items[index].sequence = ++queue->sequence;
    queue->count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_task_queue_pop(
    UmiAiCodingTaskQueue *queue,
    UmiAiCodingQueuedTask *out_task)
{
    if (queue == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (queue->count == 0U) return UMI_STATUS_NOT_FOUND;

    *out_task = queue->items[queue->head];
    (void)memset(&queue->items[queue->head], 0, sizeof(queue->items[0]));
    queue->head = (queue->head + 1U) % UMI_AI_CODING_TASK_QUEUE_CAPACITY;
    queue->count -= 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_task_queue_peek(
    const UmiAiCodingTaskQueue *queue,
    UmiAiCodingQueuedTask *out_task)
{
    if (queue == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (queue->count == 0U) return UMI_STATUS_NOT_FOUND;

    *out_task = queue->items[queue->head];
    return UMI_STATUS_OK;
}

size_t umi_ai_coding_task_queue_count(const UmiAiCodingTaskQueue *queue)
{
    return queue != NULL ? queue->count : 0U;
}

void umi_ai_coding_task_queue_clear(UmiAiCodingTaskQueue *queue)
{
    if (queue == NULL) return;

    (void)memset(queue->items, 0, sizeof(queue->items));
    queue->head = 0U;
    queue->count = 0U;
}
