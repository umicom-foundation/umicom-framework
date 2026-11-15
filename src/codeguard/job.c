/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/job.c
 *
 * PURPOSE:
 *   Run copied scan requests through Framework tasks without sharing partial findings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/codeguard/job.h"
#include "umicom/codeguard/scanner.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/platform/threading.h"
#include <stdlib.h>
#include <string.h>

struct UmiCodeGuardJob {
    char root[UMI_CODEGUARD_JOB_ROOT_CAPACITY];
    UmiCodeGuardProfile profile;
    uint64_t owner_thread;
    UmiTask *task;
    UmiMutex *mutex;
    UmiStatus operation_status;
    int operation_completed;
    UmiCodeGuardResult *result;
    UmiCodeGuardSummary summary;
};

static int umi_codeguard_job_terminal(UmiTaskState state)
{
    return state == UMI_TASK_SUCCEEDED || state == UMI_TASK_FAILED ||
        state == UMI_TASK_CANCELLED;
}

/* Worker input and output belong to the job. The canonical task token is
 * borrowed only for this synchronous scan; no UI or retained service is
 * accessed from a queue worker. Failed candidates are discarded locally. */
static UmiStatus umi_codeguard_job_run(UmiTaskContext *context, void *data)
{
    UmiCodeGuardJob *job = data;
    const UmiCancellationToken *cancellation = UmiTaskContextCancellation(context);
    UmiCodeGuardResult *candidate = NULL;
    UmiStatus status = umi_cancellation_token_is_requested(cancellation)
        ? UMI_STATUS_CANCELLED : umi_codeguard_result_create(256U, &candidate);
    if (status == UMI_STATUS_OK) {
        UmiCodeGuardConfig config = umi_codeguard_config_default(job->root);
        config.profile = job->profile;
        status = umi_codeguard_scan_cancellable(&config, candidate, cancellation);
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation))
        status = UMI_STATUS_CANCELLED;
    UmiCodeGuardSummary summary = {0};
    if (status == UMI_STATUS_OK) summary = umi_codeguard_summary_build(candidate);
    UmiStatus locked = umi_mutex_lock(job->mutex);
    if (locked != UMI_STATUS_OK) {
        umi_codeguard_result_destroy(candidate);
        return locked;
    }
    job->operation_status = status;
    job->operation_completed = 1;
    if (status == UMI_STATUS_OK) {
        job->result = candidate;
        candidate = NULL;
        job->summary = summary;
    }
    (void)umi_mutex_unlock(job->mutex);
    umi_codeguard_result_destroy(candidate);
    return status;
}

/* Creation and later adoption compare the same normalized lexical identity.
 * Resolve relative input at the call boundary; this is not filesystem identity. */
static UmiStatus umi_codeguard_job_resolve_root(const char *root,
    char absolute[UMI_CODEGUARD_JOB_ROOT_CAPACITY])
{
    if (root == NULL || root[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_PATH_CAPACITY && root[length] != '\0') ++length;
    if (length == UMI_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    char directory[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (umi_path_is_absolute(root))
        status = umi_path_normalise(root, absolute, UMI_CODEGUARD_JOB_ROOT_CAPACITY);
    else {
        status = umi_fs_current_directory(directory, sizeof directory);
        if (status == UMI_STATUS_OK)
            status = umi_path_absolute(root, directory, absolute, UMI_CODEGUARD_JOB_ROOT_CAPACITY);
    }
    if (status != UMI_STATUS_OK) return status;
    return umi_path_is_absolute(absolute) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}

/* Freeze lexical root identity before scheduling. This does not canonicalize
 * links or create a snapshot; native traversal and input own those policies. */
/* Share checked root resolution with adoption so queued findings cannot be attached through a different path interpretation. The previous implementation remains for review. */
#if 0
UmiStatus UmiCodeGuardJobCreate(const char *root, const UmiCodeGuardProfile *profile,
    UmiCodeGuardJob **out_job)
{
    if (out_job == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_job = NULL;
    if (root == NULL || root[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = 0U;
    while (length < UMI_PATH_CAPACITY && root[length] != '\0') ++length;
    if (length == UMI_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    char absolute[UMI_CODEGUARD_JOB_ROOT_CAPACITY];
    char directory[UMI_PATH_CAPACITY];
    UmiStatus status;
    if (umi_path_is_absolute(root))
        status = umi_path_normalise(root, absolute, sizeof absolute);
    else {
        status = umi_fs_current_directory(directory, sizeof directory);
        if (status == UMI_STATUS_OK)
            status = umi_path_absolute(root, directory, absolute, sizeof absolute);
    }
    if (status != UMI_STATUS_OK) return status;
    if (!umi_path_is_absolute(absolute)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardJob *job = calloc(1U, sizeof *job);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(job->root, absolute, strlen(absolute) + 1U);
    job->profile = profile != NULL ? *profile : umi_codeguard_profile_default();
    job->owner_thread = umi_thread_current_id();
    job->operation_status = UMI_STATUS_BUSY;
    status = umi_mutex_create(&job->mutex);
    if (status == UMI_STATUS_OK) {
        UmiTaskConfig task = {.label = "CodeGuard source scan",
            .function = umi_codeguard_job_run, .user_data = job};
        status = umi_task_create(&task, &job->task);
    }
    if (status != UMI_STATUS_OK) {
        umi_mutex_destroy(job->mutex);
        free(job);
        return status;
    }
    *out_job = job;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiCodeGuardJobCreate(const char *root, const UmiCodeGuardProfile *profile,
    UmiCodeGuardJob **out_job)
{
    if (out_job == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_job = NULL;
    char absolute[UMI_CODEGUARD_JOB_ROOT_CAPACITY];
    UmiStatus status = umi_codeguard_job_resolve_root(root, absolute);
    if (status != UMI_STATUS_OK) return status;
    UmiCodeGuardJob *job = calloc(1U, sizeof *job);
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(job->root, absolute, strlen(absolute) + 1U);
    job->profile = profile != NULL ? *profile : umi_codeguard_profile_default();
    job->owner_thread = umi_thread_current_id();
    job->operation_status = UMI_STATUS_BUSY;
    status = umi_mutex_create(&job->mutex);
    if (status == UMI_STATUS_OK) {
        UmiTaskConfig task = {.label = "CodeGuard source scan",
            .function = umi_codeguard_job_run, .user_data = job};
        status = umi_task_create(&task, &job->task);
    }
    if (status != UMI_STATUS_OK) {
        umi_mutex_destroy(job->mutex);
        free(job);
        return status;
    }
    *out_job = job;
    return UMI_STATUS_OK;
}

UmiStatus UmiCodeGuardJobSubmit(UmiCodeGuardJob *job, UmiTaskQueue *queue)
{
    if (job == NULL || queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (job->owner_thread != umi_thread_current_id()) return UMI_STATUS_INVALID_STATE;
    return umi_task_queue_submit(queue, job->task);
}

UmiStatus UmiCodeGuardJobCancel(UmiCodeGuardJob *job)
{
    return job != NULL ? umi_task_cancel(job->task) : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus UmiCodeGuardJobRead(UmiCodeGuardJob *job, UmiCodeGuardJobSnapshot *out)
{
    if (job == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardJobSnapshot snapshot = {.state = umi_task_state(job->task),
        .status = UMI_STATUS_BUSY};
    UmiStatus status = umi_mutex_lock(job->mutex);
    if (status != UMI_STATUS_OK) return status;
    snapshot.operation_status = job->operation_status;
    snapshot.operation_completed = job->operation_completed;
    if (umi_codeguard_job_terminal(snapshot.state)) {
        /* The generic task classifies cancellation separately. Preserve an
         * earlier worker failure as the operation's actionable cause. */
        snapshot.status = snapshot.operation_completed &&
            snapshot.operation_status != UMI_STATUS_OK
            ? snapshot.operation_status : umi_task_result(job->task);
        if (snapshot.state == UMI_TASK_SUCCEEDED &&
            snapshot.operation_completed && snapshot.operation_status == UMI_STATUS_OK) {
            snapshot.summary = job->summary;
            snapshot.result_available = job->result != NULL;
        } else if (snapshot.status == UMI_STATUS_OK)
            snapshot.status = UMI_STATUS_INVALID_STATE;
    }
    (void)umi_mutex_unlock(job->mutex);
    *out = snapshot;
    return UMI_STATUS_OK;
}

/* Terminal task state means its callback returned; only the creating thread
 * transfers or releases the result. Cancellation after terminal completion
 * is a no-op in the shared task service, preserving that publication boundary. */
UmiStatus UmiCodeGuardJobTake(UmiCodeGuardJob *job, UmiCodeGuardResult **out)
{
    if (job == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (job->owner_thread != umi_thread_current_id()) return UMI_STATUS_INVALID_STATE;
    UmiCodeGuardJobSnapshot snapshot;
    UmiStatus status = UmiCodeGuardJobRead(job, &snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (!umi_codeguard_job_terminal(snapshot.state)) return UMI_STATUS_BUSY;
    if (snapshot.status != UMI_STATUS_OK) return snapshot.status;
    status = umi_mutex_lock(job->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (job->result == NULL) status = UMI_STATUS_NOT_FOUND;
    else {
        *out = job->result;
        job->result = NULL;
    }
    (void)umi_mutex_unlock(job->mutex);
    return status;
}

UmiStatus UmiCodeGuardJobDestroy(UmiCodeGuardJob **in_out_job)
{
    if (in_out_job == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiCodeGuardJob *job = *in_out_job;
    if (job == NULL) return UMI_STATUS_OK;
    if (job->owner_thread != umi_thread_current_id()) return UMI_STATUS_INVALID_STATE;
    UmiTaskState state = umi_task_state(job->task);
    if (state == UMI_TASK_QUEUED || state == UMI_TASK_RUNNING) return UMI_STATUS_BUSY;
    umi_task_destroy(job->task);
    umi_codeguard_result_destroy(job->result);
    umi_mutex_destroy(job->mutex);
    free(job);
    *in_out_job = NULL;
    return UMI_STATUS_OK;
}

/* Immutable input can be copied by a live borrower without exposing job storage.
 * Callers exclude destruction; a failed capacity check never writes a prefix. */
UmiStatus UmiCodeGuardJobCopyRoot(const UmiCodeGuardJob *job, char *out, size_t capacity)
{
    if (job == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t required = strlen(job->root) + 1U;
    if (capacity < required) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, job->root, required);
    return UMI_STATUS_OK;
}

UmiStatus UmiCodeGuardJobMatchRoot(const UmiCodeGuardJob *job,
    const char *root, int *out_match)
{
    if (job == NULL || out_match == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char absolute[UMI_CODEGUARD_JOB_ROOT_CAPACITY];
    UmiStatus status = umi_codeguard_job_resolve_root(root, absolute);
    if (status != UMI_STATUS_OK) return status;
    *out_match = umi_path_equal(job->root, absolute);
    return UMI_STATUS_OK;
}
