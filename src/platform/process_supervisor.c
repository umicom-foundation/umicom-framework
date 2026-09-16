/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_supervisor.c
 *
 * PURPOSE:
 *   Implement asynchronous process-job ownership on top of the portable child
 *   process and threading services while preserving request argument lifetimes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/process_supervisor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/cancellation.h"
#include "umicom/platform/clock.h"
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
    size_t waiters; /* A waiting caller pins the slot until it has observed completion. */
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

/* Provide the copy string operation used by this module and its client applications. */
static UmiStatus copy_string(char *destination,
                             size_t capacity,
                             const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (source == NULL) {
        destination[0] = '\0';
        return UMI_STATUS_OK;
    }
    length = strlen(source);
    /* Compare before adding one so a hostile maximum-length string cannot wrap
     * the size calculation before the capacity check. */
    if (length >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the own request operation used by this module and its client applications. */
static UmiStatus own_request(UmiOwnedProcessRequest *owned,
                             const UmiProcessRequest *request,
                             UmiCancellationToken *cancellation)
{
    size_t index;
    UmiStatus status;
    /* Validate this private copy boundary as well as the public executor.  The
     * supervisor owns fixed arrays, so counts and pointer arrays must agree
     * before any loop can index them. */
    if (owned == NULL || request == NULL || request->program == NULL ||
        request->program[0] == '\0' ||
        request->argument_count > UMI_PROCESS_MAX_ARGUMENTS ||
        request->environment_count > UMI_PROCESS_MAX_ENVIRONMENT ||
        (request->argument_count > 0U && request->arguments == NULL) ||
        (request->environment_count > 0U && request->environment == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(owned, 0, sizeof(*owned));
    status = copy_string(owned->program,
                         sizeof(owned->program),
                         request->program);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request->working_directory != NULL) {
        status = copy_string(owned->working_directory,
                             sizeof(owned->working_directory),
                             request->working_directory);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->argument_count; ++index) {
        if (request->arguments[index] == NULL) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = copy_string(owned->arguments[index],
                             sizeof(owned->arguments[index]),
                             request->arguments[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        owned->argument_pointers[index] = owned->arguments[index];
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < request->environment_count; ++index) {
        if (request->environment[index].name == NULL ||
            request->environment[index].value == NULL ||
            request->environment[index].name[0] == '\0') {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        status = copy_string(owned->environment_names[index],
                             sizeof(owned->environment_names[index]),
                             request->environment[index].name);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = copy_string(owned->environment_values[index],
                             sizeof(owned->environment_values[index]),
                             request->environment[index].value);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/* Provide the find job operation used by this module and its client applications. */
static UmiProcessJob *find_job(UmiProcessSupervisor *supervisor,
                               UmiProcessJobId job_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < supervisor->capacity; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (job_id != 0U && supervisor->jobs[index].job_id == job_id) {
            return &supervisor->jobs[index];
        }
    }
    return NULL;
}

/* Preserve live captured output without sharing the executor's mutable buffer.
 * The observer runs on the job worker; readers copy under this same mutex. */
static void PublishProcessOutput(const UmiProcessResult *result, void *context)
{
    UmiProcessJob *job = context;
    (void)umi_mutex_lock(job->owner->mutex);
    job->result = *result;
    (void)umi_mutex_unlock(job->owner->mutex);
}

/*
 * Provide the process job thread operation used by this module and its client
 * applications.
 */
static int process_job_thread(void *user_data)
{
    UmiProcessJob *job = (UmiProcessJob *)user_data;
    UmiStatus status;
    UmiProcessResult result = {0};
    (void)umi_mutex_lock(job->owner->mutex);
    job->state = UMI_PROCESS_JOB_RUNNING;
    job->owner->stats.running += 1U;
    (void)umi_condition_broadcast(job->owner->condition);
    (void)umi_mutex_unlock(job->owner->mutex);

    /* The executor writes a worker-owned result and publishes coherent live
     * copies through its observer. Publish completion and terminal state under
     * the same mutex, retaining both live output and the final result. */
    status = UmiProcessExecuteObserved(&job->owned.request,
        PublishProcessOutput, job, &result);

    (void)umi_mutex_lock(job->owner->mutex);
    job->result = result;
    job->owner->stats.running -= 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_CANCELLED || job->result.cancelled) {
        job->state = UMI_PROCESS_JOB_CANCELLED;
        job->owner->stats.cancelled += 1U;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_TIMEOUT || job->result.timed_out) {
        job->state = UMI_PROCESS_JOB_TIMED_OUT;
        job->owner->stats.timed_out += 1U;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) {
        job->state = UMI_PROCESS_JOB_SUCCEEDED;
        job->owner->stats.succeeded += 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        job->state = UMI_PROCESS_JOB_FAILED;
        job->owner->stats.failed += 1U;
    }
    (void)umi_condition_broadcast(job->owner->condition);
    (void)umi_mutex_unlock(job->owner->mutex);
    return status == UMI_STATUS_OK ? 0 : 1;
}

/*
 * Provide the process supervisor config default operation used by this module and its
 * client applications.
 */
UmiProcessSupervisorConfig umi_process_supervisor_config_default(void)
{
    UmiProcessSupervisorConfig config;
    config.capacity = 16U;
    return config;
}

/*
 * Initialise process supervisor from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_process_supervisor_create(
    const UmiProcessSupervisorConfig *config,
    UmiProcessSupervisor **out_supervisor)
{
    UmiProcessSupervisorConfig effective;
    UmiProcessSupervisor *supervisor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_supervisor = NULL;
    effective = config != NULL
        ? *config
        : umi_process_supervisor_config_default();
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (effective.capacity == 0U ||
        effective.capacity > UMI_PROCESS_SUPERVISOR_MAX_JOBS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    supervisor = (UmiProcessSupervisor *)calloc(1U, sizeof(*supervisor));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (supervisor == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    supervisor->jobs = (UmiProcessJob *)calloc(effective.capacity,
                                               sizeof(*supervisor->jobs));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Release or reset state held by process supervisor so the same storage can be reused
 * safely.
 */
void umi_process_supervisor_destroy(UmiProcessSupervisor *supervisor)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (supervisor == NULL) return;
    (void)umi_process_supervisor_shutdown(supervisor);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < supervisor->capacity; ++index) {
        umi_thread_destroy(supervisor->jobs[index].thread);
        umi_cancellation_token_destroy(supervisor->jobs[index].cancellation);
    }
    umi_condition_destroy(supervisor->condition);
    umi_mutex_destroy(supervisor->mutex);
    free(supervisor->jobs);
    free(supervisor);
}

/*
 * Provide the process supervisor submit operation used by this module and its client
 * applications.
 */
UmiStatus umi_process_supervisor_submit(UmiProcessSupervisor *supervisor,
                                        const char *label,
                                        const UmiProcessRequest *request,
                                        UmiProcessJobId *out_job_id)
{
    UmiProcessJob *job = NULL;
    size_t index;
    UmiProcessJobId assignedId;
    UmiCancellationToken *cancellation = NULL;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (supervisor == NULL || request == NULL || out_job_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Create the cancellation object before reserving a job slot.  If it
     * cannot be allocated, the supervisor remains unchanged and another
     * caller can still use the available capacity. */
    status = umi_cancellation_token_create(&cancellation);
    if (status != UMI_STATUS_OK) return status;

    (void)umi_mutex_lock(supervisor->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (supervisor->shutting_down) {
        (void)umi_mutex_unlock(supervisor->mutex);
        umi_cancellation_token_destroy(cancellation);
        return UMI_STATUS_INVALID_STATE;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (supervisor->count >= supervisor->capacity || supervisor->next_job_id == UINT64_MAX) {
        (void)umi_mutex_unlock(supervisor->mutex);
        umi_cancellation_token_destroy(cancellation);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Keep reservation, request ownership and thread start in one critical
     * section.  This prevents a second submitter from being inserted between
     * the reservation and a failed copy, which previously left a ghost job
     * occupying capacity. */
    /* Released slots may be reused, but a live job never moves in memory:
     * its worker and its copied argv pointers refer to this exact address. */
    for (index = 0U; index < supervisor->capacity; ++index) {
        if (supervisor->jobs[index].job_id == 0U) {
            job = &supervisor->jobs[index];
            break;
        }
    }
    if (job == NULL) {
        (void)umi_mutex_unlock(supervisor->mutex);
        umi_cancellation_token_destroy(cancellation);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memset(job, 0, sizeof(*job));
    job->owner = supervisor;
    assignedId = supervisor->next_job_id++;
    job->job_id = assignedId;
    job->state = UMI_PROCESS_JOB_CREATED;
    job->cancellation = cancellation;
    status = copy_string(job->label,
                         sizeof(job->label),
                         label != NULL ? label : "process");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = own_request(&job->owned, request, cancellation);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_thread_start(process_job_thread, job, &job->thread);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        /* The thread-start contract leaves out_thread NULL on failure, so no
         * worker can still reference this token when it is released here. */
        (void)memset(job, 0, sizeof(*job));
        (void)umi_mutex_unlock(supervisor->mutex);
        umi_cancellation_token_destroy(cancellation);
        return status;
    }
    supervisor->count += 1U;
    supervisor->stats.jobs = supervisor->count;
    supervisor->stats.submitted += 1U;
    *out_job_id = assignedId;
    (void)umi_mutex_unlock(supervisor->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the process supervisor cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_process_supervisor_cancel(UmiProcessSupervisor *supervisor,
                                        UmiProcessJobId job_id)
{
    UmiProcessJob *job;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(supervisor->mutex);
    job = find_job(supervisor, job_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (job == NULL) {
        (void)umi_mutex_unlock(supervisor->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    umi_cancellation_token_request(job->cancellation);
    (void)umi_mutex_unlock(supervisor->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the process supervisor wait operation used by this module and its client
 * applications.
 */
UmiStatus umi_process_supervisor_wait(UmiProcessSupervisor *supervisor,
                                      UmiProcessJobId job_id,
                                      uint32_t timeout_ms)
{
    UmiProcessJob *job;
    UmiStatus status = UMI_STATUS_OK;
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock);
    if (supervisor == NULL || job_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(supervisor->mutex);
    job = find_job(supervisor, job_id);
    if (job == NULL) {
        (void)umi_mutex_unlock(supervisor->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    ++job->waiters;
    /* Continue only while work remains available. Notifications from other
     * jobs must not restart this caller's timeout budget. */
    while (job->state == UMI_PROCESS_JOB_CREATED ||
           job->state == UMI_PROCESS_JOB_RUNNING) {
        uint32_t remaining = timeout_ms;
        if (timeout_ms != 0U) {
            uint64_t now = clock.monotonic_nanoseconds(&clock);
            uint64_t elapsed = now >= started ? (now - started) / UINT64_C(1000000) : 0U;
            if (elapsed >= timeout_ms) { status = UMI_STATUS_TIMEOUT; break; }
            remaining = timeout_ms - (uint32_t)elapsed;
        }
        status = timeout_ms == 0U
            ? umi_condition_wait(supervisor->condition, supervisor->mutex)
            : umi_condition_wait_for(supervisor->condition, supervisor->mutex, remaining);
        if (status != UMI_STATUS_OK) break;
    }
    /* A terminal worker has already published its result and released this
     * mutex. Serialising join here prevents two waiters joining one thread,
     * and pins the slot against explicit release until this call completes. */
    if (status == UMI_STATUS_OK && job->thread != NULL && !job->joined) {
        status = umi_thread_join(job->thread, NULL);
        if (status == UMI_STATUS_OK) job->joined = 1;
    }
    --job->waiters;
    (void)umi_mutex_unlock(supervisor->mutex);
    return status;
}

/* Release only acknowledged, terminal work. No compaction is allowed while
 * another slot still contains a worker or self-referencing request pointers. */
UmiStatus UmiProcessSupervisorReleaseJob(UmiProcessSupervisor *supervisor,
    UmiProcessJobId jobId)
{
    UmiProcessJob *job;
    UmiStatus status = UMI_STATUS_OK;
    if (supervisor == NULL || jobId == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(supervisor->mutex);
    job = find_job(supervisor, jobId);
    if (supervisor->shutting_down) status = UMI_STATUS_INVALID_STATE;
    else if (job == NULL) status = UMI_STATUS_NOT_FOUND;
    else if (job->state == UMI_PROCESS_JOB_CREATED ||
             job->state == UMI_PROCESS_JOB_RUNNING || job->waiters != 0U)
        status = UMI_STATUS_BUSY;
    else {
        if (job->thread != NULL && !job->joined)
            status = umi_thread_join(job->thread, NULL);
        if (status == UMI_STATUS_OK) {
            umi_thread_destroy(job->thread);
            umi_cancellation_token_destroy(job->cancellation);
            (void)memset(job, 0, sizeof(*job));
            --supervisor->count;
            supervisor->stats.jobs = supervisor->count;
        }
    }
    (void)umi_mutex_unlock(supervisor->mutex);
    return status;
}

/* Provide the copy snapshot operation used by this module and its client applications. */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= sizeof(snapshot->output)) {
        length = sizeof(snapshot->output) - 1U;
        snapshot->output_truncated = 1;
    }
    (void)memcpy(snapshot->output, job->result.output, length);
    snapshot->output[length] = '\0';
}

/*
 * Provide the process supervisor snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_process_supervisor_snapshot(
    const UmiProcessSupervisor *supervisor,
    UmiProcessJobId job_id,
    UmiProcessJobSnapshot *out_snapshot)
{
    UmiProcessJob *job;
    UmiProcessSupervisor *mutable_supervisor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (supervisor == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    mutable_supervisor = (UmiProcessSupervisor *)supervisor;
    (void)umi_mutex_lock(mutable_supervisor->mutex);
    job = find_job(mutable_supervisor, job_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (job == NULL) {
        (void)umi_mutex_unlock(mutable_supervisor->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    copy_snapshot(job, out_snapshot);
    (void)umi_mutex_unlock(mutable_supervisor->mutex);
    return UMI_STATUS_OK;
}

/*
 * Find process supervisor while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_process_supervisor_at(const UmiProcessSupervisor *supervisor,
                                    size_t index,
                                    UmiProcessJobSnapshot *out_snapshot)
{
    UmiProcessSupervisor *mutable_supervisor;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (supervisor == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    mutable_supervisor = (UmiProcessSupervisor *)supervisor;
    (void)umi_mutex_lock(mutable_supervisor->mutex);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= supervisor->count) {
        (void)umi_mutex_unlock(mutable_supervisor->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    for (size_t slot = 0U; slot < supervisor->capacity; ++slot) {
        if (supervisor->jobs[slot].job_id == 0U) continue;
        if (index == 0U) { copy_snapshot(&supervisor->jobs[slot], out_snapshot); break; }
        --index;
    }
    (void)umi_mutex_unlock(mutable_supervisor->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the process supervisor shutdown operation used by this module and its client
 * applications.
 */
UmiStatus umi_process_supervisor_shutdown(UmiProcessSupervisor *supervisor)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(supervisor->mutex);
    /* Apply this branch only when its contract condition is satisfied. */
    if (supervisor->shutting_down) {
        (void)umi_mutex_unlock(supervisor->mutex);
        return UMI_STATUS_OK;
    }
    supervisor->shutting_down = 1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < supervisor->capacity; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (supervisor->jobs[index].job_id != 0U &&
            (supervisor->jobs[index].state == UMI_PROCESS_JOB_CREATED ||
             supervisor->jobs[index].state == UMI_PROCESS_JOB_RUNNING)) {
            umi_cancellation_token_request(supervisor->jobs[index].cancellation);
        }
    }
    (void)umi_mutex_unlock(supervisor->mutex);
    /* Shutdown rejects release/submission, so occupied slot identities stay
     * stable while wait performs the existing completion and join sequence. */
    for (index = 0U; index < supervisor->capacity; ++index) {
        if (supervisor->jobs[index].job_id != 0U) {
            UmiStatus status = umi_process_supervisor_wait(
                supervisor, supervisor->jobs[index].job_id, 0U);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the process supervisor stats operation used by this module and its client
 * applications.
 */
UmiProcessSupervisorStats umi_process_supervisor_stats(
    const UmiProcessSupervisor *supervisor)
{
    UmiProcessSupervisorStats stats;
    UmiProcessSupervisor *mutable_supervisor;
    (void)memset(&stats, 0, sizeof(stats));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (supervisor == NULL) return stats;
    mutable_supervisor = (UmiProcessSupervisor *)supervisor;
    (void)umi_mutex_lock(mutable_supervisor->mutex);
    stats = supervisor->stats;
    (void)umi_mutex_unlock(mutable_supervisor->mutex);
    return stats;
}

/*
 * Provide the process job state text operation used by this module and its client
 * applications.
 */
const char *umi_process_job_state_text(UmiProcessJobState state)
{
    /* Select the behaviour associated with the requested command or state value. */
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
