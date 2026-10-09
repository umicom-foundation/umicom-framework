/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/working_tree_job.c
 * PURPOSE: Queue repository observations using copied inputs and explicit result ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/threading.h"
#include "umicom/vcs/working_tree_job.h"
#include <stdlib.h>
#include <string.h>

struct UmiVcsWorkingTreeJob
{
    char root[UMI_VCS_PATH_CAPACITY];
    char program[UMI_VCS_PATH_CAPACITY];
    uint32_t timeout_ms;
    uint64_t owner;
    UmiTask *task;
    UmiMutex *mutex;
    UmiStatus status;
    UmiVcsWorkingTree *result;
};

/* Completion is the ownership boundary after which the worker no longer touches the result. */
static int JobTerminal(UmiTaskState state)
{
    return state == UMI_TASK_SUCCEEDED || state == UMI_TASK_FAILED || state == UMI_TASK_CANCELLED;
}

/* The worker uses copied strings and its task token, never a widget or caller-owned request. */
static UmiStatus ObserveWorkingTree(UmiTaskContext *context, void *data)
{
    UmiVcsWorkingTreeJob *job = data;
    UmiVcsWorkingTreeRequest request = {0};
    UmiVcsWorkingTree *tree = NULL;
    UmiStatus status;
    request.repository_root = job->root;
    request.git_program = job->program;
    request.timeout_ms = job->timeout_ms;
    request.cancellation = UmiTaskContextCancellation(context);
    status = UmiVcsWorkingTreeRead(&request, &tree);
    (void)umi_mutex_lock(job->mutex);
    job->status = status;
    job->result = tree;
    (void)umi_mutex_unlock(job->mutex);
    return status;
}

/* Copy all lifetime-sensitive input before the task can be submitted to another thread. */
UmiStatus UmiVcsWorkingTreeJobCreate(const UmiVcsWorkingTreeRequest *request,
                                     UmiVcsWorkingTreeJob **out_job)
{
    UmiVcsWorkingTreeJob *job;
    UmiTaskConfig config = {0};
    UmiStatus status;
    const char *program;
    if (request == NULL || out_job == NULL || request->repository_root == NULL ||
        request->repository_root[0] == '\0' || request->cancellation != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    program = request->git_program != NULL && request->git_program[0] != '\0' ? request->git_program
                                                                              : "git";
    if (strlen(program) >= UMI_VCS_PATH_CAPACITY ||
        strlen(request->repository_root) >= UMI_VCS_PATH_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    job = calloc(1U, sizeof(*job));
    if (job == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(job->root, request->repository_root, strlen(request->repository_root) + 1U);
    memcpy(job->program, program, strlen(program) + 1U);
    job->timeout_ms = request->timeout_ms;
    job->owner = umi_thread_current_id();
    job->status = UMI_STATUS_BUSY;
    status = umi_mutex_create(&job->mutex);
    if (status == UMI_STATUS_OK)
    {
        config.label = "Inspect repository working tree";
        config.function = ObserveWorkingTree;
        config.user_data = job;
        status = umi_task_create(&config, &job->task);
    }
    if (status != UMI_STATUS_OK)
    {
        umi_mutex_destroy(job->mutex);
        free(job);
        return status;
    }
    *out_job = job;
    return UMI_STATUS_OK;
}

/* Only the owner schedules the task, so queue submission and result transfer cannot race. */
UmiStatus UmiVcsWorkingTreeJobSubmit(UmiVcsWorkingTreeJob *job, UmiTaskQueue *queue)
{
    if (job == NULL || queue == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (job->owner != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    return umi_task_queue_submit(queue, job->task);
}

/* Cancellation is thread-safe; the job must remain alive until it reaches a terminal state. */
UmiStatus UmiVcsWorkingTreeJobCancel(UmiVcsWorkingTreeJob *job)
{
    return job != NULL ? umi_task_cancel(job->task) : UMI_STATUS_INVALID_ARGUMENT;
}

/* Sample task state first, then copy protected worker data without retaining any borrowed pointer.
 */
UmiStatus UmiVcsWorkingTreeJobRead(UmiVcsWorkingTreeJob *job, UmiVcsWorkingTreeJobSnapshot *out)
{
    UmiVcsWorkingTreeJobSnapshot snapshot;
    if (job == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    snapshot.state = umi_task_state(job->task);
    (void)umi_mutex_lock(job->mutex);
    snapshot.status = job->status;
    (void)umi_mutex_unlock(job->mutex);
    if (JobTerminal(snapshot.state))
        snapshot.status = umi_task_result(job->task);
    *out = snapshot;
    return UMI_STATUS_OK;
}

/* Transfer ownership once; no result is exposed while cancellation or process completion is
 * pending. */
UmiStatus UmiVcsWorkingTreeJobTake(UmiVcsWorkingTreeJob *job, UmiVcsWorkingTree **out_tree)
{
    UmiVcsWorkingTreeJobSnapshot snapshot;
    UmiStatus status;
    if (job == NULL || out_tree == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (job->owner != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    status = UmiVcsWorkingTreeJobRead(job, &snapshot);
    if (status != UMI_STATUS_OK)
        return status;
    if (!JobTerminal(snapshot.state))
        return UMI_STATUS_BUSY;
    if (snapshot.state == UMI_TASK_CANCELLED)
        return UMI_STATUS_CANCELLED;
    if (snapshot.status != UMI_STATUS_OK)
        return snapshot.status;
    if (job->result == NULL)
        return UMI_STATUS_NOT_FOUND;
    *out_tree = job->result;
    job->result = NULL;
    return UMI_STATUS_OK;
}

/* Refuse early destruction instead of releasing memory still borrowed by the worker. */
UmiStatus UmiVcsWorkingTreeJobDestroy(UmiVcsWorkingTreeJob *job)
{
    UmiTaskState state;
    if (job == NULL)
        return UMI_STATUS_OK;
    if (job->owner != umi_thread_current_id())
        return UMI_STATUS_INVALID_STATE;
    state = umi_task_state(job->task);
    if (state == UMI_TASK_QUEUED || state == UMI_TASK_RUNNING)
        return UMI_STATUS_BUSY;
    umi_task_destroy(job->task);
    umi_mutex_destroy(job->mutex);
    UmiVcsWorkingTreeDestroy(job->result);
    free(job);
    return UMI_STATUS_OK;
}
