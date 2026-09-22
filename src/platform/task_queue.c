/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/task_queue.c
 *
 * PURPOSE:
 *   Implement a bounded, thread-safe task queue with worker threads, explicit
 *   shutdown semantics and aggregate execution statistics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/task_queue.h"

#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"
#include "task_wait.inc"

#define UMI_TASK_QUEUE_MAX_WORKERS 64U
#define UMI_TASK_QUEUE_MAX_CAPACITY 65536U

typedef struct UmiTaskQueueWorkerSlot {
    struct UmiTaskQueue *queue;
    size_t index;
} UmiTaskQueueWorkerSlot;

/* Prevent a worker from joining its own queue or waiting for its own idleness. */
static _Thread_local struct UmiTaskQueue *umiExecutingQueue;

struct UmiTaskQueue {
    UmiTask **items;
    UmiThread **workers;
    UmiTask *activeTasks[UMI_TASK_QUEUE_MAX_WORKERS];
    UmiTaskQueueWorkerSlot workerSlots[UMI_TASK_QUEUE_MAX_WORKERS];
    int workerJoined[UMI_TASK_QUEUE_MAX_WORKERS];
    int joining;
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

/* Provide the task queue worker operation used by this module and its client applications. */
static int umi_task_queue_worker(void *user_data)
{
    UmiTaskQueueWorkerSlot *slot = (UmiTaskQueueWorkerSlot *)user_data;
    UmiTaskQueue *queue = slot->queue;
    umiExecutingQueue = queue;
    /* Visit each bounded item once so every record receives the same rule. */
    for (;;) {
        UmiTask *task;
        UmiStatus result;
        (void)umi_mutex_lock(queue->mutex);
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (queue->queued == 0U && !queue->stopping) {
            (void)umi_condition_wait(queue->not_empty, queue->mutex);
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (queue->queued == 0U && queue->stopping) {
            (void)umi_mutex_unlock(queue->mutex);
            break;
        }
        task = queue->items[queue->head];
        queue->items[queue->head] = NULL;
        queue->head = (queue->head + 1U) % queue->capacity;
        queue->queued -= 1U;
        queue->running += 1U;
        queue->activeTasks[slot->index] = task;
        (void)umi_condition_signal(queue->not_full);
        (void)umi_mutex_unlock(queue->mutex);

        result = umi_task_run(task);

        (void)umi_mutex_lock(queue->mutex);
        queue->activeTasks[slot->index] = NULL;
        /* A cancelled queued task may already have lost its caller reference.
         * Release the queue reference only after its final pointer use. */
        umi_task_destroy(task);
        queue->running -= 1U;
        queue->completed += 1U;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (result == UMI_STATUS_CANCELLED) queue->cancelled += 1U;
        else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (result != UMI_STATUS_OK) queue->failed += 1U;
        /* Apply this branch only when its contract condition is satisfied. */
        if (queue->queued == 0U && queue->running == 0U) {
            (void)umi_condition_broadcast(queue->idle);
        }
        (void)umi_mutex_unlock(queue->mutex);
    }
    umiExecutingQueue = NULL;
    return 0;
}

/*
 * Provide the task queue config default operation used by this module and its client
 * applications.
 */
UmiTaskQueueConfig umi_task_queue_config_default(void)
{
    UmiTaskQueueConfig config;
    config.worker_count = 2U;
    config.capacity = 256U;
    return config;
}

/*
 * Initialise task queue from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_task_queue_create(const UmiTaskQueueConfig *config,
                                UmiTaskQueue **out_queue)
{
    UmiTaskQueueConfig effective;
    UmiTaskQueue *queue;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_queue = NULL;
    effective = config != NULL ? *config : umi_task_queue_config_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.worker_count == 0U ||
        effective.worker_count > UMI_TASK_QUEUE_MAX_WORKERS ||
        effective.capacity == 0U ||
        effective.capacity > UMI_TASK_QUEUE_MAX_CAPACITY) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    queue = (UmiTaskQueue *)calloc(1U, sizeof(*queue));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    queue->items = (UmiTask **)calloc(effective.capacity,
                                      sizeof(*queue->items));
    queue->workers = (UmiThread **)calloc(effective.worker_count,
                                           sizeof(*queue->workers));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < queue->worker_count; ++index) {
        queue->workerSlots[index].queue = queue;
        queue->workerSlots[index].index = index;
        UmiStatus status = umi_thread_start(umi_task_queue_worker,
                                            &queue->workerSlots[index],
                                            &queue->workers[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/* Release or reset state held by task queue so the same storage can be reused safely. */
void umi_task_queue_destroy(UmiTaskQueue *queue)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return;
    /* Destruction from a worker cannot join itself. Keep storage alive rather
     * than freeing a queue still in use; only its external owner may destroy. */
    if (umiExecutingQueue == queue) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!queue->stopped && queue->worker_count > 0U && queue->mutex != NULL) {
        if (umi_task_queue_shutdown(queue, 1) != UMI_STATUS_OK) return;
    }
    /* Visit each bounded item once so every record receives the same rule. */
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

/* Provide the task queue submit operation used by this module and its client applications. */
UmiStatus umi_task_queue_submit(UmiTaskQueue *queue, UmiTask *task)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(queue->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (queue->stopping) {
        (void)umi_mutex_unlock(queue->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (queue->queued >= queue->capacity) {
        (void)umi_mutex_unlock(queue->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = UmiTaskRetain(task);
    if (status == UMI_STATUS_OK) {
        status = umi_task_mark_queued(task);
        if (status != UMI_STATUS_OK) umi_task_destroy(task);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)umi_mutex_unlock(queue->mutex);
        return status;
    }
    queue->items[queue->tail] = task;
    queue->tail = (queue->tail + 1U) % queue->capacity;
    queue->queued += 1U;
    queue->submitted += 1U;
    (void)umi_condition_signal(queue->not_empty);
    (void)umi_mutex_unlock(queue->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the task queue wait idle operation used by this module and its client
 * applications.
 */
UmiStatus umi_task_queue_wait_idle(UmiTaskQueue *queue, uint32_t timeout_ms)
{
    UmiStatus status = UMI_STATUS_OK;
    UmiClock clock = umi_clock_system();
    uint64_t startTime = clock.monotonic_nanoseconds(&clock);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umiExecutingQueue == queue) return UMI_STATUS_INVALID_STATE;
    (void)umi_mutex_lock(queue->mutex);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (queue->queued > 0U || queue->running > 0U) {
        uint32_t remaining = UmiTaskWaitRemaining(&clock, startTime, timeout_ms);
        if (timeout_ms != 0U && remaining == 0U) {
            status = UMI_STATUS_TIMEOUT;
            break;
        }
        status = timeout_ms == 0U
            ? umi_condition_wait(queue->idle, queue->mutex)
            : umi_condition_wait_for(queue->idle, queue->mutex, remaining);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
    }
    (void)umi_mutex_unlock(queue->mutex);
    return status;
}

/* queue->mutex serialises the transition with worker dequeue/completion.
 * No user callback runs here. Active pointers retain a queue-owned reference. */
static void UmiTaskQueueCancelLocked(UmiTaskQueue *queue,
    int cancelPending, int cancelRunning)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (cancelPending) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (queue->queued > 0U) {
            UmiTask *task = queue->items[queue->head];
            queue->items[queue->head] = NULL;
            queue->head = (queue->head + 1U) % queue->capacity;
            queue->queued -= 1U;
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (task != NULL) {
                (void)umi_task_cancel(task);
                umi_task_destroy(task);
            }
            queue->cancelled += 1U;
            queue->completed += 1U;
        }
    }
    if (cancelRunning) {
        for (size_t index = 0U; index < queue->worker_count; ++index) {
            if (queue->activeTasks[index] != NULL)
                (void)umi_task_cancel(queue->activeTasks[index]);
        }
    }
    if (queue->queued == 0U && queue->running == 0U)
        (void)umi_condition_broadcast(queue->idle);
    (void)umi_condition_broadcast(queue->not_full);
}

UmiStatus UmiTaskQueueCancelAll(UmiTaskQueue *queue)
{
    if (queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(queue->mutex);
    UmiTaskQueueCancelLocked(queue, 1, 1);
    (void)umi_mutex_unlock(queue->mutex);
    return UMI_STATUS_OK;
}

UmiStatus UmiTaskQueueRequestShutdown(UmiTaskQueue *queue,
    int cancelPending, int cancelRunning)
{
    if (queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(queue->mutex);
    queue->stopping = 1;
    UmiTaskQueueCancelLocked(queue, cancelPending, cancelRunning);
    (void)umi_condition_broadcast(queue->not_empty);
    (void)umi_mutex_unlock(queue->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the task queue shutdown operation used by this module and its client
 * applications.
 */
UmiStatus umi_task_queue_shutdown(UmiTaskQueue *queue, int cancel_pending)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umiExecutingQueue == queue) return UMI_STATUS_INVALID_STATE;
    status = UmiTaskQueueRequestShutdown(queue, cancel_pending, 0);
    if (status != UMI_STATUS_OK) return status;
    (void)umi_mutex_lock(queue->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (queue->stopped) {
        (void)umi_mutex_unlock(queue->mutex);
        return UMI_STATUS_OK;
    }
    if (queue->joining) {
        (void)umi_mutex_unlock(queue->mutex);
        return UMI_STATUS_BUSY;
    }
    queue->joining = 1;
    (void)umi_mutex_unlock(queue->mutex);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < queue->worker_count; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (queue->workers[index] != NULL && !queue->workerJoined[index]) {
            status = umi_thread_join(queue->workers[index], NULL);
            if (status != UMI_STATUS_OK) break;
            queue->workerJoined[index] = 1;
        }
    }
    (void)umi_mutex_lock(queue->mutex);
    if (status == UMI_STATUS_OK) queue->stopped = 1;
    queue->joining = 0;
    (void)umi_mutex_unlock(queue->mutex);
    return status;
}

/* Provide the task queue stats operation used by this module and its client applications. */
UmiTaskQueueStats umi_task_queue_stats(const UmiTaskQueue *queue)
{
    UmiTaskQueueStats stats;
    UmiTaskQueue *mutable_queue;

    (void)memset(&stats, 0, sizeof(stats));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
