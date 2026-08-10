/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/task_queue.c
 *
 * PURPOSE:
 *   Implement a bounded, thread-safe task queue with worker threads, explicit
 *   shutdown semantics and aggregate execution statistics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/task_queue.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

#define UMI_TASK_QUEUE_MAX_WORKERS 64U
#define UMI_TASK_QUEUE_MAX_CAPACITY 65536U

struct UmiTaskQueue {
    UmiTask **items;
    UmiThread **workers;
    size_t worker_count;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t queued;
    size_t running;
    uint64_t submitted;
    uint64_t completed;
    uint64_t cancelled;
    uint64_t failed;
    int stopping;
    int stopped;
    UmiMutex *mutex;
    UmiCondition *not_empty;
    UmiCondition *not_full;
    UmiCondition *idle;
};

static int umi_task_queue_worker(void *user_data)
{
    UmiTaskQueue *queue = (UmiTaskQueue *)user_data;
    for (;;) {
        UmiTask *task;
        UmiStatus result;
        (void)umi_mutex_lock(queue->mutex);
        while (queue->queued == 0U && !queue->stopping) {
            (void)umi_condition_wait(queue->not_empty, queue->mutex);
        }
        if (queue->queued == 0U && queue->stopping) {
            (void)umi_mutex_unlock(queue->mutex);
            break;
        }
        task = queue->items[queue->head];
        queue->items[queue->head] = NULL;
        queue->head = (queue->head + 1U) % queue->capacity;
        queue->queued -= 1U;
        queue->running += 1U;
        (void)umi_condition_signal(queue->not_full);
        (void)umi_mutex_unlock(queue->mutex);

        result = umi_task_run(task);

        (void)umi_mutex_lock(queue->mutex);
        queue->running -= 1U;
        queue->completed += 1U;
        if (result == UMI_STATUS_CANCELLED) queue->cancelled += 1U;
        else if (result != UMI_STATUS_OK) queue->failed += 1U;
        if (queue->queued == 0U && queue->running == 0U) {
            (void)umi_condition_broadcast(queue->idle);
        }
        (void)umi_mutex_unlock(queue->mutex);
    }
    return 0;
}

UmiTaskQueueConfig umi_task_queue_config_default(void)
{
    UmiTaskQueueConfig config;
    config.worker_count = 2U;
    config.capacity = 256U;
    return config;
}

UmiStatus umi_task_queue_create(const UmiTaskQueueConfig *config,
                                UmiTaskQueue **out_queue)
{
    UmiTaskQueueConfig effective;
    UmiTaskQueue *queue;
    size_t index;
    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;
    effective = config != NULL ? *config : umi_task_queue_config_default();
    if (effective.worker_count == 0U ||
        effective.worker_count > UMI_TASK_QUEUE_MAX_WORKERS ||
        effective.capacity == 0U ||
        effective.capacity > UMI_TASK_QUEUE_MAX_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    queue = (UmiTaskQueue *)calloc(1U, sizeof(*queue));
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    queue->items = (UmiTask **)calloc(effective.capacity,
                                      sizeof(*queue->items));
    queue->workers = (UmiThread **)calloc(effective.worker_count,
                                           sizeof(*queue->workers));
    if (queue->items == NULL || queue->workers == NULL ||
        umi_mutex_create(&queue->mutex) != UMI_STATUS_OK ||
        umi_condition_create(&queue->not_empty) != UMI_STATUS_OK ||
        umi_condition_create(&queue->not_full) != UMI_STATUS_OK ||
        umi_condition_create(&queue->idle) != UMI_STATUS_OK) {
        umi_task_queue_destroy(queue);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    queue->worker_count = effective.worker_count;
    queue->capacity = effective.capacity;
    for (index = 0U; index < queue->worker_count; ++index) {
        UmiStatus status = umi_thread_start(umi_task_queue_worker,
                                            queue,
                                            &queue->workers[index]);
        if (status != UMI_STATUS_OK) {
            queue->worker_count = index;
            (void)umi_task_queue_shutdown(queue, 1);
            umi_task_queue_destroy(queue);
            return status;
        }
    }
    *out_queue = queue;
    return UMI_STATUS_OK;
}

void umi_task_queue_destroy(UmiTaskQueue *queue)
{
    size_t index;
    if (queue == NULL) return;
    if (!queue->stopped && queue->worker_count > 0U && queue->mutex != NULL) {
        (void)umi_task_queue_shutdown(queue, 1);
    }
    for (index = 0U; index < queue->worker_count; ++index) {
        umi_thread_destroy(queue->workers[index]);
    }
    umi_condition_destroy(queue->idle);
    umi_condition_destroy(queue->not_full);
    umi_condition_destroy(queue->not_empty);
    umi_mutex_destroy(queue->mutex);
    free(queue->workers);
    free(queue->items);
    free(queue);
}

UmiStatus umi_task_queue_submit(UmiTaskQueue *queue, UmiTask *task)
{
    UmiStatus status;
    if (queue == NULL || task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_task_mark_queued(task);
    if (status != UMI_STATUS_OK) return status;
    (void)umi_mutex_lock(queue->mutex);
    if (queue->stopping) {
        (void)umi_mutex_unlock(queue->mutex);
        (void)umi_task_cancel(task);
        return UMI_STATUS_INVALID_STATE;
    }
    if (queue->queued >= queue->capacity) {
        (void)umi_mutex_unlock(queue->mutex);
        (void)umi_task_cancel(task);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    queue->items[queue->tail] = task;
    queue->tail = (queue->tail + 1U) % queue->capacity;
    queue->queued += 1U;
    queue->submitted += 1U;
    (void)umi_condition_signal(queue->not_empty);
    (void)umi_mutex_unlock(queue->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_task_queue_wait_idle(UmiTaskQueue *queue, uint32_t timeout_ms)
{
    UmiStatus status = UMI_STATUS_OK;
    if (queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(queue->mutex);
    while (queue->queued > 0U || queue->running > 0U) {
        status = timeout_ms == 0U
            ? umi_condition_wait(queue->idle, queue->mutex)
            : umi_condition_wait_for(queue->idle, queue->mutex, timeout_ms);
        if (status != UMI_STATUS_OK) break;
    }
    (void)umi_mutex_unlock(queue->mutex);
    return status;
}

UmiStatus umi_task_queue_shutdown(UmiTaskQueue *queue, int cancel_pending)
{
    size_t index;
    if (queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(queue->mutex);
    if (queue->stopped) {
        (void)umi_mutex_unlock(queue->mutex);
        return UMI_STATUS_OK;
    }
    queue->stopping = 1;
    if (cancel_pending) {
        while (queue->queued > 0U) {
            UmiTask *task = queue->items[queue->head];
            queue->items[queue->head] = NULL;
            queue->head = (queue->head + 1U) % queue->capacity;
            queue->queued -= 1U;
            if (task != NULL) (void)umi_task_cancel(task);
            queue->cancelled += 1U;
            queue->completed += 1U;
        }
    }
    (void)umi_condition_broadcast(queue->not_empty);
    (void)umi_condition_broadcast(queue->not_full);
    (void)umi_mutex_unlock(queue->mutex);

    for (index = 0U; index < queue->worker_count; ++index) {
        if (queue->workers[index] != NULL) {
            (void)umi_thread_join(queue->workers[index], NULL);
        }
    }
    queue->stopped = 1;
    return UMI_STATUS_OK;
}

UmiTaskQueueStats umi_task_queue_stats(const UmiTaskQueue *queue)
{
    UmiTaskQueueStats stats;
    UmiTaskQueue *mutable_queue;

    (void)memset(&stats, 0, sizeof(stats));
    if (queue == NULL) return stats;

    mutable_queue = (UmiTaskQueue *)queue;
    (void)umi_mutex_lock(mutable_queue->mutex);
    stats.worker_count = queue->worker_count;
    stats.capacity = queue->capacity;
    stats.queued = queue->queued;
    stats.running = queue->running;
    stats.submitted = queue->submitted;
    stats.completed = queue->completed;
    stats.cancelled = queue->cancelled;
    stats.failed = queue->failed;
    (void)umi_mutex_unlock(mutable_queue->mutex);
    return stats;
}
