/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/task.c
 *
 * PURPOSE:
 *   Implement cancellable task state, progress reporting and completion
 *   waiting independently of the worker or queue that executes the task.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/task.h"

#include <stdatomic.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "umicom/platform/threading.h"
#include "task_wait.inc"

#define UMI_TASK_LABEL_CAPACITY 160U

struct UmiTaskContext {
    struct UmiTask *task;
};

struct UmiTask {
    uint64_t id;
    char label[UMI_TASK_LABEL_CAPACITY];
    UmiTaskFunction function;
    void *user_data;
    UmiTaskProgressSink progress_sink;
    void *progress_user_data;
    UmiMutex *mutex;
    UmiCondition *condition;
    atomic_uint referenceCount;
    UmiCancellationToken *cancellation;
    UmiTaskState state;
    UmiStatus result;
    unsigned progress;
};

static atomic_uint_fast64_t g_next_task_id = 1U;

/* Initialise task from caller-provided values so later operations receive a known state. */
UmiStatus umi_task_create(const UmiTaskConfig *config,
                          UmiTask **out_task)
{
    UmiTask *task;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || config->function == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_task = NULL;
    task = (UmiTask *)calloc(1U, sizeof(*task));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    atomic_init(&task->referenceCount, 1U);
    task->id = atomic_fetch_add(&g_next_task_id, 1U);
    (void)snprintf(task->label,
                   sizeof(task->label),
                   "%s",
                   config->label != NULL ? config->label : "task");
    task->function = config->function;
    task->user_data = config->user_data;
    task->progress_sink = config->progress_sink;
    task->progress_user_data = config->progress_user_data;
    task->state = UMI_TASK_CREATED;
    task->result = UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    status = umi_mutex_create(&task->mutex);
    if (status == UMI_STATUS_OK) status = umi_condition_create(&task->condition);
    if (status == UMI_STATUS_OK)
        status = umi_cancellation_token_create(&task->cancellation);
    if (status != UMI_STATUS_OK) {
        umi_task_destroy(task);
        return status;
    }
    *out_task = task;
    return UMI_STATUS_OK;
}

/* Release or reset state held by task so the same storage can be reused safely. */
void umi_task_destroy(UmiTask *task)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task == NULL) return;
    /* One acquire-release operation makes the final owner's hand-off explicit
     * before destroying locks, cancellation state or storage. */
    if (atomic_fetch_sub_explicit(&task->referenceCount, 1U,
                                  memory_order_acq_rel) != 1U) return;
    umi_cancellation_token_destroy(task->cancellation);
    umi_condition_destroy(task->condition);
    umi_mutex_destroy(task->mutex);
    free(task);
}

/* References protect task storage, not caller-owned callback payloads. */
UmiStatus UmiTaskRetain(UmiTask *task)
{
    unsigned references;
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    references = atomic_load_explicit(&task->referenceCount, memory_order_relaxed);
    do {
        if (references == 0U) return UMI_STATUS_INVALID_STATE;
        if (references == UINT_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    } while (!atomic_compare_exchange_weak_explicit(&task->referenceCount,
        &references, references + 1U, memory_order_relaxed, memory_order_relaxed));
    return UMI_STATUS_OK;
}

/* Provide the task mark queued operation used by this module and its client applications. */
UmiStatus umi_task_mark_queued(UmiTask *task)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(task->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (task->state != UMI_TASK_CREATED) {
        (void)umi_mutex_unlock(task->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    task->state = UMI_TASK_QUEUED;
    (void)umi_mutex_unlock(task->mutex);
    return UMI_STATUS_OK;
}

/*
 * Perform task through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_task_run(UmiTask *task)
{
    UmiTaskContext context;
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    result = UmiTaskRetain(task);
    if (result != UMI_STATUS_OK) return result;

    (void)umi_mutex_lock(task->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (task->state == UMI_TASK_CANCELLED) {
        (void)umi_mutex_unlock(task->mutex);
        umi_task_destroy(task); /* release only the runner's reference */
        return UMI_STATUS_CANCELLED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (task->state != UMI_TASK_CREATED && task->state != UMI_TASK_QUEUED) {
        (void)umi_mutex_unlock(task->mutex);
        umi_task_destroy(task);
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_cancellation_token_is_requested(task->cancellation)) {
        task->state = UMI_TASK_CANCELLED;
        task->result = UMI_STATUS_CANCELLED;
        (void)umi_condition_broadcast(task->condition);
        (void)umi_mutex_unlock(task->mutex);
        umi_task_destroy(task); /* release only the runner's reference */
        return UMI_STATUS_CANCELLED;
    }
    task->state = UMI_TASK_RUNNING;
    (void)umi_mutex_unlock(task->mutex);

    context.task = task;
    result = task->function(&context, task->user_data);

    (void)umi_mutex_lock(task->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_cancellation_token_is_requested(task->cancellation) ||
        result == UMI_STATUS_CANCELLED) {
        task->state = UMI_TASK_CANCELLED;
        task->result = UMI_STATUS_CANCELLED;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (result == UMI_STATUS_OK) {
        task->state = UMI_TASK_SUCCEEDED;
        task->result = UMI_STATUS_OK;
        task->progress = 100U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        task->state = UMI_TASK_FAILED;
        task->result = result;
    }
    /* A waiter can release a completed task as soon as this mutex is unlocked.
     * Copy the final result before waking it; do not read task fields afterwards.
     * The runner now owns a reference, which it releases after unlocking. */
    result = task->result;
    (void)umi_condition_broadcast(task->condition);
    (void)umi_mutex_unlock(task->mutex);
    umi_task_destroy(task);
    return result;
}

/* Provide the task cancel operation used by this module and its client applications. */
UmiStatus umi_task_cancel(UmiTask *task)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(task->mutex);
    /* Linearise cancellation with completion. A late Stop never changes a
     * completed result or marks its token as cancelled afterwards. */
    if (task->state == UMI_TASK_CREATED || task->state == UMI_TASK_QUEUED ||
        task->state == UMI_TASK_RUNNING) {
        umi_cancellation_token_request(task->cancellation);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (task->state == UMI_TASK_CREATED || task->state == UMI_TASK_QUEUED) {
        task->state = UMI_TASK_CANCELLED;
        task->result = UMI_STATUS_CANCELLED;
        (void)umi_condition_broadcast(task->condition);
    }
    (void)umi_mutex_unlock(task->mutex);
    return UMI_STATUS_OK;
}

/* Provide the task wait operation used by this module and its client applications. */
UmiStatus umi_task_wait(UmiTask *task, uint32_t timeout_ms)
{
    UmiStatus wait_status = UMI_STATUS_OK;
    UmiClock clock = umi_clock_system();
    uint64_t startTime = clock.monotonic_nanoseconds(&clock);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(task->mutex);
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (task->state == UMI_TASK_CREATED ||
           task->state == UMI_TASK_QUEUED ||
           task->state == UMI_TASK_RUNNING) {
        uint32_t remaining = UmiTaskWaitRemaining(&clock, startTime, timeout_ms);
        if (timeout_ms != 0U && remaining == 0U) {
            wait_status = UMI_STATUS_TIMEOUT;
            break;
        }
        wait_status = timeout_ms == 0U
            ? umi_condition_wait(task->condition, task->mutex)
            : umi_condition_wait_for(task->condition,
                                     task->mutex,
                                     remaining);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (wait_status != UMI_STATUS_OK) break;
    }
    (void)umi_mutex_unlock(task->mutex);
    return wait_status;
}

/* Provide the task state operation used by this module and its client applications. */
UmiTaskState umi_task_state(const UmiTask *task)
{
    UmiTaskState state;
    UmiTask *mutable_task;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task == NULL) return UMI_TASK_FAILED;
    mutable_task = (UmiTask *)task;
    (void)umi_mutex_lock(mutable_task->mutex);
    state = task->state;
    (void)umi_mutex_unlock(mutable_task->mutex);
    return state;
}

/* Provide the task result operation used by this module and its client applications. */
UmiStatus umi_task_result(const UmiTask *task)
{
    UmiStatus result;
    UmiTask *mutable_task;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    mutable_task = (UmiTask *)task;
    (void)umi_mutex_lock(mutable_task->mutex);
    result = task->result;
    (void)umi_mutex_unlock(mutable_task->mutex);
    return result;
}

/* Provide the task id operation used by this module and its client applications. */
uint64_t umi_task_id(const UmiTask *task)
{
    return task != NULL ? task->id : 0U;
}

/* Provide the task label operation used by this module and its client applications. */
const char *umi_task_label(const UmiTask *task)
{
    return task != NULL ? task->label : "";
}

/* Provide the task progress operation used by this module and its client applications. */
unsigned umi_task_progress(const UmiTask *task)
{
    unsigned progress;
    UmiTask *mutable_task;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task == NULL) return 0U;
    mutable_task = (UmiTask *)task;
    (void)umi_mutex_lock(mutable_task->mutex);
    progress = task->progress;
    (void)umi_mutex_unlock(mutable_task->mutex);
    return progress;
}

/*
 * Provide the task context is cancelled operation used by this module and its client
 * applications.
 */
int umi_task_context_is_cancelled(const UmiTaskContext *context)
{
    return context != NULL && context->task != NULL &&
        umi_cancellation_token_is_requested(context->task->cancellation);
}

/*
 * Provide the task context report operation used by this module and its client
 * applications.
 */
UmiStatus umi_task_context_report(UmiTaskContext *context,
                                  unsigned progress_percent,
                                  const char *message)
{
    UmiTask *task;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || context->task == NULL || progress_percent > 100U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    task = context->task;
    (void)umi_mutex_lock(task->mutex);
    task->progress = progress_percent;
    (void)umi_mutex_unlock(task->mutex);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task->progress_sink != NULL) {
        task->progress_sink(task->id,
                            progress_percent,
                            message != NULL ? message : "",
                            task->progress_user_data);
    }
    return umi_task_context_is_cancelled(context)
        ? UMI_STATUS_CANCELLED
        : UMI_STATUS_OK;
}

/* Provide the task context id operation used by this module and its client applications. */
uint64_t umi_task_context_id(const UmiTaskContext *context)
{
    return context != NULL && context->task != NULL
        ? context->task->id
        : 0U;
}

const UmiCancellationToken *UmiTaskContextCancellation(
    const UmiTaskContext *context)
{
    return context != NULL && context->task != NULL
        ? context->task->cancellation : NULL;
}
