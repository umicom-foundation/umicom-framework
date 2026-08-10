/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/task.c
 *
 * PURPOSE:
 *   Implement cancellable task state, progress reporting and completion
 *   waiting independently of the worker or queue that executes the task.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/task.h"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#include "umicom/platform/threading.h"

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
    atomic_int cancel_requested;
    UmiTaskState state;
    UmiStatus result;
    unsigned progress;
};

static atomic_uint_fast64_t g_next_task_id = 1U;

UmiStatus umi_task_create(const UmiTaskConfig *config,
                          UmiTask **out_task)
{
    UmiTask *task;
    if (config == NULL || config->function == NULL || out_task == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_task = NULL;
    task = (UmiTask *)calloc(1U, sizeof(*task));
    if (task == NULL) return UMI_STATUS_OUT_OF_MEMORY;
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
    atomic_init(&task->cancel_requested, 0);
    if (umi_mutex_create(&task->mutex) != UMI_STATUS_OK ||
        umi_condition_create(&task->condition) != UMI_STATUS_OK) {
        umi_task_destroy(task);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *out_task = task;
    return UMI_STATUS_OK;
}

void umi_task_destroy(UmiTask *task)
{
    if (task == NULL) return;
    umi_condition_destroy(task->condition);
    umi_mutex_destroy(task->mutex);
    free(task);
}

UmiStatus umi_task_mark_queued(UmiTask *task)
{
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(task->mutex);
    if (task->state != UMI_TASK_CREATED) {
        (void)umi_mutex_unlock(task->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    task->state = UMI_TASK_QUEUED;
    (void)umi_mutex_unlock(task->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_task_run(UmiTask *task)
{
    UmiTaskContext context;
    UmiStatus result;
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    (void)umi_mutex_lock(task->mutex);
    if (task->state == UMI_TASK_CANCELLED) {
        (void)umi_mutex_unlock(task->mutex);
        return UMI_STATUS_CANCELLED;
    }
    if (task->state != UMI_TASK_CREATED && task->state != UMI_TASK_QUEUED) {
        (void)umi_mutex_unlock(task->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    if (atomic_load(&task->cancel_requested) != 0) {
        task->state = UMI_TASK_CANCELLED;
        task->result = UMI_STATUS_CANCELLED;
        (void)umi_condition_broadcast(task->condition);
        (void)umi_mutex_unlock(task->mutex);
        return UMI_STATUS_CANCELLED;
    }
    task->state = UMI_TASK_RUNNING;
    (void)umi_mutex_unlock(task->mutex);

    context.task = task;
    result = task->function(&context, task->user_data);

    (void)umi_mutex_lock(task->mutex);
    if (atomic_load(&task->cancel_requested) != 0 ||
        result == UMI_STATUS_CANCELLED) {
        task->state = UMI_TASK_CANCELLED;
        task->result = UMI_STATUS_CANCELLED;
    } else if (result == UMI_STATUS_OK) {
        task->state = UMI_TASK_SUCCEEDED;
        task->result = UMI_STATUS_OK;
        task->progress = 100U;
    } else {
        task->state = UMI_TASK_FAILED;
        task->result = result;
    }
    (void)umi_condition_broadcast(task->condition);
    (void)umi_mutex_unlock(task->mutex);
    return task->result;
}

UmiStatus umi_task_cancel(UmiTask *task)
{
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    atomic_store(&task->cancel_requested, 1);
    (void)umi_mutex_lock(task->mutex);
    if (task->state == UMI_TASK_CREATED || task->state == UMI_TASK_QUEUED) {
        task->state = UMI_TASK_CANCELLED;
        task->result = UMI_STATUS_CANCELLED;
        (void)umi_condition_broadcast(task->condition);
    }
    (void)umi_mutex_unlock(task->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_task_wait(UmiTask *task, uint32_t timeout_ms)
{
    UmiStatus wait_status = UMI_STATUS_OK;
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(task->mutex);
    while (task->state == UMI_TASK_CREATED ||
           task->state == UMI_TASK_QUEUED ||
           task->state == UMI_TASK_RUNNING) {
        wait_status = timeout_ms == 0U
            ? umi_condition_wait(task->condition, task->mutex)
            : umi_condition_wait_for(task->condition,
                                     task->mutex,
                                     timeout_ms);
        if (wait_status != UMI_STATUS_OK) break;
    }
    (void)umi_mutex_unlock(task->mutex);
    return wait_status;
}

UmiTaskState umi_task_state(const UmiTask *task)
{
    UmiTaskState state;
    UmiTask *mutable_task;
    if (task == NULL) return UMI_TASK_FAILED;
    mutable_task = (UmiTask *)task;
    (void)umi_mutex_lock(mutable_task->mutex);
    state = task->state;
    (void)umi_mutex_unlock(mutable_task->mutex);
    return state;
}

UmiStatus umi_task_result(const UmiTask *task)
{
    UmiStatus result;
    UmiTask *mutable_task;
    if (task == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    mutable_task = (UmiTask *)task;
    (void)umi_mutex_lock(mutable_task->mutex);
    result = task->result;
    (void)umi_mutex_unlock(mutable_task->mutex);
    return result;
}

uint64_t umi_task_id(const UmiTask *task)
{
    return task != NULL ? task->id : 0U;
}

const char *umi_task_label(const UmiTask *task)
{
    return task != NULL ? task->label : "";
}

unsigned umi_task_progress(const UmiTask *task)
{
    unsigned progress;
    UmiTask *mutable_task;
    if (task == NULL) return 0U;
    mutable_task = (UmiTask *)task;
    (void)umi_mutex_lock(mutable_task->mutex);
    progress = task->progress;
    (void)umi_mutex_unlock(mutable_task->mutex);
    return progress;
}

int umi_task_context_is_cancelled(const UmiTaskContext *context)
{
    return context != NULL && context->task != NULL &&
        atomic_load(&context->task->cancel_requested) != 0;
}

UmiStatus umi_task_context_report(UmiTaskContext *context,
                                  unsigned progress_percent,
                                  const char *message)
{
    UmiTask *task;
    if (context == NULL || context->task == NULL || progress_percent > 100U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    task = context->task;
    (void)umi_mutex_lock(task->mutex);
    task->progress = progress_percent;
    (void)umi_mutex_unlock(task->mutex);
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

uint64_t umi_task_context_id(const UmiTaskContext *context)
{
    return context != NULL && context->task != NULL
        ? context->task->id
        : 0U;
}
