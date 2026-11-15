/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/task_queue.c
 *
 * PURPOSE:
 *   Implement heap-owned FIFO coding-task storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Initialise ai coding task queue from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ai_coding_task_queue_create(UmiAiCodingTaskQueue **out_queue)
{
    UmiAiCodingTaskQueue *queue;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;

    queue = (UmiAiCodingTaskQueue *)calloc(1U, sizeof(*queue));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_queue = queue;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ai coding task queue so the same storage can be reused
 * safely.
 */
void umi_ai_coding_task_queue_destroy(UmiAiCodingTaskQueue *queue)
{
    free(queue);
}

/*
 * Provide the ai coding task queue push operation used by this module and its client
 * applications.
 */
/* Centralize single and batch scheduling so malformed drafts, capacity failures and exhausted identities cannot partially change the queue. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_task_queue_push(
    UmiAiCodingTaskQueue *queue,
    const UmiAiCodingQueuedTask *task)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || task == NULL ||
        task->task_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (queue->count >= UMI_AI_CODING_TASK_QUEUE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    index = (queue->head + queue->count) % UMI_AI_CODING_TASK_QUEUE_CAPACITY;
    queue->items[index] = *task;
    queue->items[index].sequence = ++queue->sequence;
    queue->count += 1U;
    return UMI_STATUS_OK;
}

#endif
/* The opaque owner must describe a bounded ring before any index is used.
 * This is an invariant check, not a way to recover a stale or invalid pointer. */
static UmiStatus task_queue_check(const UmiAiCodingTaskQueue *queue)
{
    if (queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (queue->head >= UMI_AI_CODING_TASK_QUEUE_CAPACITY ||
        queue->count > UMI_AI_CODING_TASK_QUEUE_CAPACITY)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

/* Validate the whole input before touching owner state. The copied requests
 * remain drafts; the agent performs full readiness and permission checks when
 * executing them. Sequence values are assigned here and never wrap or reset. */
UmiStatus UmiAiCodingTaskQueuePushBatch(UmiAiCodingTaskQueue *queue,
    const UmiAiCodingQueuedTask *tasks, size_t task_count)
{
    UmiStatus status = task_queue_check(queue);
    if (status != UMI_STATUS_OK) return status;
    if (tasks == NULL && task_count != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (task_count > UMI_AI_CODING_TASK_QUEUE_CAPACITY - queue->count ||
        task_count > UINT64_MAX - queue->sequence)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < task_count; ++i) {
        if (memchr(tasks[i].task_id, '\0', sizeof tasks[i].task_id) == NULL ||
            tasks[i].task_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
        status = UmiAiCodingRequestValidateText(&tasks[i].request);
        if (status != UMI_STATUS_OK) return status;
    }
    for (size_t i = 0U; i < task_count; ++i) {
        size_t index = (queue->head + queue->count) % UMI_AI_CODING_TASK_QUEUE_CAPACITY;
        queue->items[index] = tasks[i];
        queue->items[index].sequence = ++queue->sequence;
        ++queue->count;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_task_queue_push(
    UmiAiCodingTaskQueue *queue,
    const UmiAiCodingQueuedTask *task)
{
    return UmiAiCodingTaskQueuePushBatch(queue, task, 1U);
}

/*
 * Provide the ai coding task queue pop operation used by this module and its client
 * applications.
 */
/* Check ring bounds before reading the front task and preserve the caller result on refusal. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_task_queue_pop(
    UmiAiCodingTaskQueue *queue,
    UmiAiCodingQueuedTask *out_task)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (queue->count == 0U) return UMI_STATUS_NOT_FOUND;

    *out_task = queue->items[queue->head];
    (void)memset(&queue->items[queue->head], 0, sizeof(queue->items[0]));
    queue->head = (queue->head + 1U) % UMI_AI_CODING_TASK_QUEUE_CAPACITY;
    queue->count -= 1U;
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_task_queue_pop(
    UmiAiCodingTaskQueue *queue,
    UmiAiCodingQueuedTask *out_task)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    UmiStatus owner_status = task_queue_check(queue);
    if (owner_status != UMI_STATUS_OK) return owner_status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (queue->count == 0U) return UMI_STATUS_NOT_FOUND;

    *out_task = queue->items[queue->head];
    (void)memset(&queue->items[queue->head], 0, sizeof(queue->items[0]));
    queue->head = (queue->head + 1U) % UMI_AI_CODING_TASK_QUEUE_CAPACITY;
    queue->count -= 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ai coding task queue peek operation used by this module and its client
 * applications.
 */
/* Check ring bounds before reading the front task and preserve the caller result on refusal. The former implementation is retained for owner review. */
#if 0
UmiStatus umi_ai_coding_task_queue_peek(
    const UmiAiCodingTaskQueue *queue,
    UmiAiCodingQueuedTask *out_task)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (queue->count == 0U) return UMI_STATUS_NOT_FOUND;

    *out_task = queue->items[queue->head];
    return UMI_STATUS_OK;
}

#endif
UmiStatus umi_ai_coding_task_queue_peek(
    const UmiAiCodingTaskQueue *queue,
    UmiAiCodingQueuedTask *out_task)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    UmiStatus owner_status = task_queue_check(queue);
    if (owner_status != UMI_STATUS_OK) return owner_status;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (queue->count == 0U) return UMI_STATUS_NOT_FOUND;

    *out_task = queue->items[queue->head];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by ai coding task queue without changing their
 * state.
 */
size_t umi_ai_coding_task_queue_count(const UmiAiCodingTaskQueue *queue)
{
    return queue != NULL ? queue->count : 0U;
}

/*
 * Release or reset state held by ai coding task queue so the same storage can be reused
 * safely.
 */
void umi_ai_coding_task_queue_clear(UmiAiCodingTaskQueue *queue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return;

    (void)memset(queue->items, 0, sizeof(queue->items));
    queue->head = 0U;
    queue->count = 0U;
}

/* Construct a stable draft before an execution owner clears prior task data.
 * Bounds and text validation belong to Framework; permission and readiness
 * remain the later assistant preparation contract. Staging also permits the
 * input fields to alias the caller's destination without losing their bytes. */
UmiStatus UmiAiCodingQueuedTaskPrepare(const char *task_id,
    const UmiAiCodingRequest *request, UmiAiCodingQueuedTask *out_task)
{
    if (task_id == NULL || request == NULL || out_task == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_AI_CODING_RUNTIME_ID_CAPACITY && task_id[length] != '\0')
        ++length;
    if (length == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (length == UMI_AI_CODING_RUNTIME_ID_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = UmiAiCodingRequestValidateText(request);
    if (status != UMI_STATUS_OK) return status;
    UmiAiCodingQueuedTask candidate = {0};
    memcpy(candidate.task_id, task_id, length + 1U);
    candidate.request = *request;
    *out_task = candidate;
    return UMI_STATUS_OK;
}
