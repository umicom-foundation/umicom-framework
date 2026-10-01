/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_job.inc
 * PURPOSE: Execute CTest attempts using the existing task and verified adapter.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/ctest_job.h"
#include "umicom/platform/threading.h"

struct UmiCtestJob {
    UmiTask *task;
    UmiMutex *mutex;
    uint64_t owner_thread;
    UmiCtestJobRequest *requests;
    UmiTestResult *results;
    size_t count;
    uint32_t repeats;
    int stop_on_failure;
    UmiCtestJobSnapshot progress;
};

static int CtestJobTerminal(UmiTaskState state)
{
    return state == UMI_TASK_SUCCEEDED || state == UMI_TASK_FAILED ||
        state == UMI_TASK_CANCELLED;
}

/* Record one completed result under the job lock. Readers never inspect the
 * slot while the provider is writing it. No application registry is borrowed. */
static void CtestJobRecord(UmiCtestJob *job, const UmiTestResult *result)
{
    (void)umi_mutex_lock(job->mutex);
    job->results[job->progress.completed++] = *result;
    switch (result->state) {
        case UMI_TEST_STATE_PASSED: ++job->progress.passed; break;
        case UMI_TEST_STATE_FAILED: ++job->progress.failed; break;
        case UMI_TEST_STATE_SKIPPED: ++job->progress.skipped; break;
        case UMI_TEST_STATE_CANCELLED: ++job->progress.cancelled; break;
        case UMI_TEST_STATE_TIMED_OUT: ++job->progress.timed_out; break;
        default: ++job->progress.not_run; break;
    }
    if (UINT64_MAX - job->progress.duration_ms < result->duration_ms)
        job->progress.duration_ms = UINT64_MAX;
    else job->progress.duration_ms += result->duration_ms;
    if (job->progress.first_error == UMI_STATUS_OK && result->status != UMI_STATUS_OK)
        job->progress.first_error = result->status;
    job->progress.active_test_id[0] = '\0';
    job->progress.active_name[0] = '\0';
    job->progress.active_attempt = 0U;
    (void)umi_mutex_unlock(job->mutex);
}

/* One worker serialises the selected attempts; CTest retains fixture and
 * timeout ownership. The GUI thread can cancel or read progress throughout. */
static UmiStatus CtestJobRun(UmiTaskContext *context, void *data)
{
    UmiCtestJob *job = data;
    UmiStatus firstError = UMI_STATUS_OK;
    size_t completed = 0U;
    for (uint32_t pass = 0U; pass < job->repeats; ++pass) {
        for (size_t index = 0U; index < job->count; ++index) {
            const UmiCtestJobRequest *request = &job->requests[index];
            if (umi_task_context_is_cancelled(context)) return UMI_STATUS_CANCELLED;
            (void)umi_mutex_lock(job->mutex);
            memcpy(job->progress.active_test_id, request->test_id, sizeof(request->test_id));
            memcpy(job->progress.active_name, request->name, sizeof(request->name));
            job->progress.active_attempt = pass + 1U;
            (void)umi_mutex_unlock(job->mutex);
            UmiTestResult result = {0};
            memcpy(result.test_id, request->test_id, sizeof(result.test_id));
            memcpy(result.name, request->name, sizeof(result.name));
            result.state = UMI_TEST_STATE_NOT_RUN;
            result.exit_code = -1;
            if (!request->enabled) {
                result.state = UMI_TEST_STATE_SKIPPED;
                (void)snprintf(result.output, sizeof(result.output),
                    "Disabled in the selected catalogue; no test process was launched.");
            } else {
                UmiCtestRunOptions options = {0};
                options.configuration = request->configuration;
                options.test_id = request->test_id;
                options.timeout_ms = request->timeout_ms;
                options.cancellation = UmiTaskContextCancellation(context);
                result.status = UmiCtestRunConfigured(request->build_directory,
                    request->name, &options, &result);
            }
            /* Do not accept a passing outcome when process/report validation
             * failed, even if a future provider change supplies inconsistent data. */
            if (result.status != UMI_STATUS_OK && result.state == UMI_TEST_STATE_PASSED)
                result.state = UMI_TEST_STATE_FAILED;
            CtestJobRecord(job, &result);
            ++completed;
            (void)umi_task_context_report(context,
                (unsigned)((completed * 100U) / job->progress.planned), request->name);
            if (firstError == UMI_STATUS_OK && result.status != UMI_STATUS_OK)
                firstError = result.status;
            if (result.state == UMI_TEST_STATE_CANCELLED ||
                umi_task_context_is_cancelled(context)) return UMI_STATUS_CANCELLED;
            if (job->stop_on_failure &&
                (result.status != UMI_STATUS_OK ||
                 (result.state != UMI_TEST_STATE_PASSED && result.state != UMI_TEST_STATE_SKIPPED)))
                return firstError != UMI_STATUS_OK ? firstError : UMI_STATUS_INVALID_STATE;
        }
    }
    return firstError;
}

/* Fully bounded validation precedes allocation and command execution. */
UmiStatus UmiCtestJobCreate(const UmiCtestJobRequest *requests, size_t count,
    const UmiCtestJobOptions *options, UmiCtestJob **outJob)
{
    if (outJob == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outJob = NULL;
    if (requests == NULL || count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    uint32_t repeats = options != NULL && options->repeat_count != 0U
        ? options->repeat_count : 1U;
    if (count > UMI_CTEST_JOB_MAX_ATTEMPTS ||
        repeats > UMI_CTEST_JOB_MAX_ATTEMPTS / count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t index = 0U; index < count; ++index) {
        const UmiCtestJobRequest *request = &requests[index];
        if (memchr(request->test_id, '\0', sizeof(request->test_id)) == NULL ||
            memchr(request->name, '\0', sizeof(request->name)) == NULL ||
            memchr(request->build_directory, '\0', sizeof(request->build_directory)) == NULL ||
            memchr(request->configuration, '\0', sizeof(request->configuration)) == NULL)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        if (request->test_id[0] == '\0' || request->name[0] == '\0' ||
            request->build_directory[0] == '\0' ||
            (request->enabled != 0 && request->enabled != 1)) return UMI_STATUS_INVALID_ARGUMENT;
        char pattern[UMI_CTEST_EXACT_PATTERN_CAPACITY];
        UmiStatus status = UmiCtestEscapeName(request->name, pattern, sizeof(pattern));
        if (status != UMI_STATUS_OK) return status;
        for (size_t prior = 0U; prior < index; ++prior)
            if (strcmp(requests[prior].test_id, request->test_id) == 0)
                return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiCtestJob *job = calloc(1U, sizeof(*job));
    if (job == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    job->owner_thread = umi_thread_current_id();
    job->count = count;
    job->repeats = repeats;
    job->stop_on_failure = options != NULL && options->stop_on_failure != 0;
    job->progress.planned = count * (size_t)repeats;
    job->requests = calloc(count, sizeof(*job->requests));
    job->results = calloc(job->progress.planned, sizeof(*job->results));
    UmiStatus status = job->requests != NULL && job->results != NULL
        ? umi_mutex_create(&job->mutex) : UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK) {
        memcpy(job->requests, requests, count * sizeof(*requests));
        UmiTaskConfig config = {0};
        config.label = "CTest selection";
        config.function = CtestJobRun;
        config.user_data = job;
        status = umi_task_create(&config, &job->task);
    }
    if (status != UMI_STATUS_OK) {
        umi_mutex_destroy(job->mutex);
        free(job->requests); free(job->results); free(job);
        return status;
    }
    *outJob = job;
    return UMI_STATUS_OK;
}

UmiStatus UmiCtestJobSubmit(UmiCtestJob *job, UmiTaskQueue *queue)
{
    if (job == NULL || queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (job->owner_thread != umi_thread_current_id()) return UMI_STATUS_INVALID_STATE;
    return umi_task_queue_submit(queue, job->task);
}

UmiStatus UmiCtestJobCancel(UmiCtestJob *job)
{
    return job != NULL ? umi_task_cancel(job->task) : UMI_STATUS_INVALID_ARGUMENT;
}

UmiStatus UmiCtestJobGetSnapshot(UmiCtestJob *job, UmiCtestJobSnapshot *outSnapshot)
{
    if (job == NULL || outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Capture task state first. A RUNNING snapshot may contain the last row,
     * but a terminal snapshot always follows all callback writes. */
    UmiTaskState state = umi_task_state(job->task);
    (void)umi_mutex_lock(job->mutex);
    *outSnapshot = job->progress;
    (void)umi_mutex_unlock(job->mutex);
    outSnapshot->task_id = umi_task_id(job->task);
    outSnapshot->state = state;
    outSnapshot->status = CtestJobTerminal(state) ? umi_task_result(job->task) : UMI_STATUS_BUSY;
    return UMI_STATUS_OK;
}

UmiStatus UmiCtestJobResultAt(UmiCtestJob *job, size_t index,
    UmiTestResult *outResult, uint32_t *outAttempt)
{
    if (job == NULL || outResult == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(job->mutex);
    if (index >= job->progress.completed) {
        (void)umi_mutex_unlock(job->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *outResult = job->results[index];
    if (outAttempt != NULL) *outAttempt = (uint32_t)(index / job->count) + 1U;
    (void)umi_mutex_unlock(job->mutex);
    return UMI_STATUS_OK;
}

UmiStatus UmiCtestJobDestroy(UmiCtestJob *job)
{
    if (job == NULL) return UMI_STATUS_OK;
    if (job->owner_thread != umi_thread_current_id()) return UMI_STATUS_INVALID_STATE;
    UmiTaskState state = umi_task_state(job->task);
    if (state == UMI_TASK_QUEUED || state == UMI_TASK_RUNNING) return UMI_STATUS_BUSY;
    umi_task_destroy(job->task);
    umi_mutex_destroy(job->mutex);
    free(job->requests); free(job->results); free(job);
    return UMI_STATUS_OK;
}
