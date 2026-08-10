/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_supervisor.c
 *
 * PURPOSE:
 *   Implement asynchronous process-job ownership on top of the portable child
 *   process and threading services while preserving request argument lifetimes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/process_supervisor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/cancellation.h"
#include "umicom/platform/threading.h"

#define UMI_JOB_STRING_CAPACITY 2048U
#define UMI_JOB_ARGUMENT_CAPACITY 1024U
#define UMI_JOB_ENVIRONMENT_CAPACITY 2048U

typedef struct UmiOwnedProcessRequest {
    UmiProcessRequest request;
    char program[UMI_JOB_STRING_CAPACITY];
    char working_directory[UMI_JOB_STRING_CAPACITY];
    char arguments[UMI_PROCESS_MAX_ARGUMENTS][UMI_JOB_ARGUMENT_CAPACITY];
    const char *argument_pointers[UMI_PROCESS_MAX_ARGUMENTS];
    char environment_names[UMI_PROCESS_MAX_ENVIRONMENT][256];
    char environment_values[UMI_PROCESS_MAX_ENVIRONMENT]
                           [UMI_JOB_ENVIRONMENT_CAPACITY];
    UmiEnvironmentVariable environment[UMI_PROCESS_MAX_ENVIRONMENT];
} UmiOwnedProcessRequest;

typedef struct UmiProcessJob {
    struct UmiProcessSupervisor *owner;
    UmiProcessJobId job_id;
    char label[UMI_PROCESS_JOB_LABEL_CAPACITY];
    UmiProcessJobState state;
    UmiOwnedProcessRequest owned;
    UmiCancellationToken *cancellation;
    UmiThread *thread;
    UmiProcessResult result;
    int joined;
} UmiProcessJob;

struct UmiProcessSupervisor {
    UmiProcessJob *jobs;
    size_t count;
    size_t capacity;
    UmiProcessJobId next_job_id;
    UmiMutex *mutex;
    UmiCondition *condition;
    UmiProcessSupervisorStats stats;
    int shutting_down;
};

static UmiStatus copy_string(char *destination,
                             size_t capacity,
                             const char *source)
{
    if (source == NULL) {
        destination[0] = '\0';
        return UMI_STATUS_OK;
    }
    if (strlen(source) + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)strcpy(destination, source);
    return UMI_STATUS_OK;
}

static UmiStatus own_request(UmiOwnedProcessRequest *owned,
                             const UmiProcessRequest *request,
                             UmiCancellationToken *cancellation)
{
    size_t index;
    UmiStatus status;
    (void)memset(owned, 0, sizeof(*owned));
    status = copy_string(owned->program,
                         sizeof(owned->program),
                         request->program);
    if (status != UMI_STATUS_OK) return status;
    if (request->working_directory != NULL) {
        status = copy_string(owned->working_directory,
                             sizeof(owned->working_directory),
                             request->working_directory);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < request->argument_count; ++index) {
        status = copy_string(owned->arguments[index],
                             sizeof(owned->arguments[index]),
                             request->arguments[index]);
        if (status != UMI_STATUS_OK) return status;
        owned->argument_pointers[index] = owned->arguments[index];
    }
    for (index = 0U; index < request->environment_count; ++index) {
        status = copy_string(owned->environment_names[index],
                             sizeof(owned->environment_names[index]),
                             request->environment[index].name);
        if (status != UMI_STATUS_OK) return status;
        status = copy_string(owned->environment_values[index],
                             sizeof(owned->environment_values[index]),
                             request->environment[index].value);
        if (status != UMI_STATUS_OK) return status;
        owned->environment[index].name = owned->environment_names[index];
        owned->environment[index].value = owned->environment_values[index];
    }
    owned->request = *request;
    owned->request.program = owned->program;
    owned->request.arguments = owned->argument_pointers;
    owned->request.working_directory =
        owned->working_directory[0] != '\0'
            ? owned->working_directory
            : NULL;
    owned->request.environment = owned->environment;
    owned->request.cancellation = cancellation;
    return UMI_STATUS_OK;
}

static UmiProcessJob *find_job(UmiProcessSupervisor *supervisor,
                               UmiProcessJobId job_id)
{
    size_t index;
    for (index = 0U; index < supervisor->count; ++index) {
        if (supervisor->jobs[index].job_id == job_id) {
            return &supervisor->jobs[index];
        }
    }
    return NULL;
}

static int process_job_thread(void *user_data)
{
    UmiProcessJob *job = (UmiProcessJob *)user_data;
    UmiStatus status;
    (void)umi_mutex_lock(job->owner->mutex);
    job->state = UMI_PROCESS_JOB_RUNNING;
    job->owner->stats.running += 1U;
    (void)umi_condition_broadcast(job->owner->condition);
    (void)umi_mutex_unlock(job->owner->mutex);

    status = umi_process_execute(&job->owned.request, &job->result);

    (void)umi_mutex_lock(job->owner->mutex);
    job->owner->stats.running -= 1U;
    if (status == UMI_STATUS_CANCELLED || job->result.cancelled) {
        job->state = UMI_PROCESS_JOB_CANCELLED;
        job->owner->stats.cancelled += 1U;
    } else if (status == UMI_STATUS_TIMEOUT || job->result.timed_out) {
        job->state = UMI_PROCESS_JOB_TIMED_OUT;
        job->owner->stats.timed_out += 1U;
    } else if (status == UMI_STATUS_OK) {
        job->state = UMI_PROCESS_JOB_SUCCEEDED;
        job->owner->stats.succeeded += 1U;
    } else {
        job->state = UMI_PROCESS_JOB_FAILED;
        job->owner->stats.failed += 1U;
    }
    (void)umi_condition_broadcast(job->owner->condition);
    (void)umi_mutex_unlock(job->owner->mutex);
    return status == UMI_STATUS_OK ? 0 : 1;
}

UmiProcessSupervisorConfig umi_process_supervisor_config_default(void)
{
    UmiProcessSupervisorConfig config;
    config.capacity = 16U;
    return config;
}

UmiStatus umi_process_supervisor_create(
    const UmiProcessSupervisorConfig *config,
    UmiProcessSupervisor **out_supervisor)
{
    UmiProcessSupervisorConfig effective;
    UmiProcessSupervisor *supervisor;
    if (out_supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_supervisor = NULL;
    effective = config != NULL
        ? *config
        : umi_process_supervisor_config_default();
    if (effective.capacity == 0U ||
        effective.capacity > UMI_PROCESS_SUPERVISOR_MAX_JOBS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    supervisor = (UmiProcessSupervisor *)calloc(1U, sizeof(*supervisor));
    if (supervisor == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    supervisor->jobs = (UmiProcessJob *)calloc(effective.capacity,
                                               sizeof(*supervisor->jobs));
    if (supervisor->jobs == NULL ||
        umi_mutex_create(&supervisor->mutex) != UMI_STATUS_OK ||
        umi_condition_create(&supervisor->condition) != UMI_STATUS_OK) {
        umi_process_supervisor_destroy(supervisor);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    supervisor->capacity = effective.capacity;
    supervisor->next_job_id = 1U;
    supervisor->stats.capacity = effective.capacity;
    *out_supervisor = supervisor;
    return UMI_STATUS_OK;
}

void umi_process_supervisor_destroy(UmiProcessSupervisor *supervisor)
{
    size_t index;
    if (supervisor == NULL) return;
    (void)umi_process_supervisor_shutdown(supervisor);
    for (index = 0U; index < supervisor->count; ++index) {
        umi_thread_destroy(supervisor->jobs[index].thread);
        umi_cancellation_token_destroy(supervisor->jobs[index].cancellation);
    }
    umi_condition_destroy(supervisor->condition);
    umi_mutex_destroy(supervisor->mutex);
    free(supervisor->jobs);
    free(supervisor);
}

UmiStatus umi_process_supervisor_submit(UmiProcessSupervisor *supervisor,
                                        const char *label,
                                        const UmiProcessRequest *request,
                                        UmiProcessJobId *out_job_id)
{
    UmiProcessJob *job;
    UmiStatus status;
    if (supervisor == NULL || request == NULL || out_job_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(supervisor->mutex);
    if (supervisor->shutting_down) {
        (void)umi_mutex_unlock(supervisor->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    if (supervisor->count >= supervisor->capacity) {
        (void)umi_mutex_unlock(supervisor->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    job = &supervisor->jobs[supervisor->count++];
    (void)memset(job, 0, sizeof(*job));
    job->owner = supervisor;
    job->job_id = supervisor->next_job_id++;
    job->state = UMI_PROCESS_JOB_CREATED;
    (void)snprintf(job->label,
                   sizeof(job->label),
                   "%s",
                   label != NULL ? label : "process");
    supervisor->stats.jobs = supervisor->count;
    supervisor->stats.submitted += 1U;
    (void)umi_mutex_unlock(supervisor->mutex);

    status = umi_cancellation_token_create(&job->cancellation);
    if (status == UMI_STATUS_OK) {
        status = own_request(&job->owned, request, job->cancellation);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_thread_start(process_job_thread, job, &job->thread);
    }
    if (status != UMI_STATUS_OK) {
        umi_cancellation_token_destroy(job->cancellation);
        job->cancellation = NULL;
        (void)umi_mutex_lock(supervisor->mutex);
        if (supervisor->count > 0U &&
            &supervisor->jobs[supervisor->count - 1U] == job) {
            supervisor->count -= 1U;
            supervisor->stats.jobs = supervisor->count;
            supervisor->stats.submitted -= 1U;
            (void)memset(job, 0, sizeof(*job));
        }
        (void)umi_mutex_unlock(supervisor->mutex);
        return status;
    }
    *out_job_id = job->job_id;
    return UMI_STATUS_OK;
}

UmiStatus umi_process_supervisor_cancel(UmiProcessSupervisor *supervisor,
                                        UmiProcessJobId job_id)
{
    UmiProcessJob *job;
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(supervisor->mutex);
    job = find_job(supervisor, job_id);
    if (job == NULL) {
        (void)umi_mutex_unlock(supervisor->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    umi_cancellation_token_request(job->cancellation);
    (void)umi_mutex_unlock(supervisor->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_process_supervisor_wait(UmiProcessSupervisor *supervisor,
                                      UmiProcessJobId job_id,
                                      uint32_t timeout_ms)
{
    UmiProcessJob *job;
    UmiStatus status = UMI_STATUS_OK;
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(supervisor->mutex);
    job = find_job(supervisor, job_id);
    if (job == NULL) {
        (void)umi_mutex_unlock(supervisor->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    while (job->state == UMI_PROCESS_JOB_CREATED ||
           job->state == UMI_PROCESS_JOB_RUNNING) {
        status = timeout_ms == 0U
            ? umi_condition_wait(supervisor->condition, supervisor->mutex)
            : umi_condition_wait_for(supervisor->condition,
                                     supervisor->mutex,
                                     timeout_ms);
        if (status != UMI_STATUS_OK) break;
    }
    (void)umi_mutex_unlock(supervisor->mutex);
    if (status == UMI_STATUS_OK && job->thread != NULL && !job->joined) {
        (void)umi_thread_join(job->thread, NULL);
        job->joined = 1;
    }
    return status;
}

static void copy_snapshot(const UmiProcessJob *job,
                          UmiProcessJobSnapshot *snapshot)
{
    size_t length;
    (void)memset(snapshot, 0, sizeof(*snapshot));
    snapshot->job_id = job->job_id;
    snapshot->state = job->state;
    snapshot->exit_code = job->result.exit_code;
    snapshot->duration_ms = job->result.duration_ms;
    snapshot->output_truncated = job->result.output_truncated;
    (void)snprintf(snapshot->label, sizeof(snapshot->label), "%s", job->label);
    length = strlen(job->result.output);
    if (length >= sizeof(snapshot->output)) {
        length = sizeof(snapshot->output) - 1U;
        snapshot->output_truncated = 1;
    }
    (void)memcpy(snapshot->output, job->result.output, length);
    snapshot->output[length] = '\0';
}

UmiStatus umi_process_supervisor_snapshot(
    const UmiProcessSupervisor *supervisor,
    UmiProcessJobId job_id,
    UmiProcessJobSnapshot *out_snapshot)
{
    UmiProcessJob *job;
    UmiProcessSupervisor *mutable_supervisor;
    if (supervisor == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    mutable_supervisor = (UmiProcessSupervisor *)supervisor;
    (void)umi_mutex_lock(mutable_supervisor->mutex);
    job = find_job(mutable_supervisor, job_id);
    if (job == NULL) {
        (void)umi_mutex_unlock(mutable_supervisor->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    copy_snapshot(job, out_snapshot);
    (void)umi_mutex_unlock(mutable_supervisor->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_process_supervisor_at(const UmiProcessSupervisor *supervisor,
                                    size_t index,
                                    UmiProcessJobSnapshot *out_snapshot)
{
    UmiProcessSupervisor *mutable_supervisor;
    if (supervisor == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    mutable_supervisor = (UmiProcessSupervisor *)supervisor;
    (void)umi_mutex_lock(mutable_supervisor->mutex);
    if (index >= supervisor->count) {
        (void)umi_mutex_unlock(mutable_supervisor->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    copy_snapshot(&supervisor->jobs[index], out_snapshot);
    (void)umi_mutex_unlock(mutable_supervisor->mutex);
    return UMI_STATUS_OK;
}

UmiStatus umi_process_supervisor_shutdown(UmiProcessSupervisor *supervisor)
{
    size_t index;
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(supervisor->mutex);
    if (supervisor->shutting_down) {
        (void)umi_mutex_unlock(supervisor->mutex);
        return UMI_STATUS_OK;
    }
    supervisor->shutting_down = 1;
    for (index = 0U; index < supervisor->count; ++index) {
        if (supervisor->jobs[index].state == UMI_PROCESS_JOB_CREATED ||
            supervisor->jobs[index].state == UMI_PROCESS_JOB_RUNNING) {
            umi_cancellation_token_request(supervisor->jobs[index].cancellation);
        }
    }
    (void)umi_mutex_unlock(supervisor->mutex);
    for (index = 0U; index < supervisor->count; ++index) {
        if (supervisor->jobs[index].thread != NULL &&
            !supervisor->jobs[index].joined) {
            (void)umi_thread_join(supervisor->jobs[index].thread, NULL);
            supervisor->jobs[index].joined = 1;
        }
    }
    return UMI_STATUS_OK;
}

UmiProcessSupervisorStats umi_process_supervisor_stats(
    const UmiProcessSupervisor *supervisor)
{
    UmiProcessSupervisorStats stats;
    UmiProcessSupervisor *mutable_supervisor;
    (void)memset(&stats, 0, sizeof(stats));
    if (supervisor == NULL) return stats;
    mutable_supervisor = (UmiProcessSupervisor *)supervisor;
    (void)umi_mutex_lock(mutable_supervisor->mutex);
    stats = supervisor->stats;
    (void)umi_mutex_unlock(mutable_supervisor->mutex);
    return stats;
}

const char *umi_process_job_state_text(UmiProcessJobState state)
{
    switch (state) {
        case UMI_PROCESS_JOB_CREATED: return "created";
        case UMI_PROCESS_JOB_RUNNING: return "running";
        case UMI_PROCESS_JOB_SUCCEEDED: return "succeeded";
        case UMI_PROCESS_JOB_FAILED: return "failed";
        case UMI_PROCESS_JOB_CANCELLED: return "cancelled";
        case UMI_PROCESS_JOB_TIMED_OUT: return "timed-out";
        default: return "unknown";
    }
}
