/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/process_supervisor_lifecycle.inc
 *
 * PURPOSE:
 *   Preserve process job owners across incomplete shutdown and release attempts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static UmiStatus SupervisorJoinJob(UmiProcessJob *job)
{
    if (job->thread == NULL || job->joined) return UMI_STATUS_OK;
    UmiStatus status = umi_thread_join(job->thread, NULL);
    /* UNAVAILABLE reports a joined native thread without an integer result.
     * Preserve that diagnostic while recording the consumed join obligation. */
    if (status == UMI_STATUS_OK || status == UMI_STATUS_UNAVAILABLE)
        job->joined = 1;
    return status;
}

UmiStatus umi_process_supervisor_shutdown(UmiProcessSupervisor *supervisor)
{
    UmiStatus status;
    if (supervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (supervisor->mutex == NULL) {
        /* A private construction failure has published no job or worker. */
        return supervisor->capacity == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    }
    status = umi_mutex_lock(supervisor->mutex);
    if (status != UMI_STATUS_OK) return status;
    /* Admission closure is not a completion flag. Every attempt verifies the
     * remaining join obligations, including after an earlier wait failed. */
    supervisor->shutting_down = 1;
    for (size_t index = 0U; index < supervisor->capacity; ++index) {
        UmiProcessJob *job = &supervisor->jobs[index];
        if (job->job_id != 0U &&
            (job->state == UMI_PROCESS_JOB_CREATED ||
             job->state == UMI_PROCESS_JOB_RUNNING))
            umi_cancellation_token_request(job->cancellation);
    }
    status = umi_mutex_unlock(supervisor->mutex);
    if (status != UMI_STATUS_OK) return status;
    /* Submission and job release are refused after admission closes. Slot
     * identities therefore remain stable during these existing checked waits. */
    for (size_t index = 0U; index < supervisor->capacity; ++index) {
        if (supervisor->jobs[index].job_id != 0U) {
            status = umi_process_supervisor_wait(
                supervisor, supervisor->jobs[index].job_id, 0U);
            if (status != UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiProcessSupervisorRelease(UmiProcessSupervisor **inOutSupervisor)
{
    UmiProcessSupervisor *supervisor;
    UmiStatus status;
    if (inOutSupervisor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    supervisor = *inOutSupervisor;
    if (supervisor == NULL) return UMI_STATUS_OK;
    status = umi_process_supervisor_shutdown(supervisor);
    if (status != UMI_STATUS_OK) return status;
    /* The owner excludes all other API users before this release. Close every
     * handle before freeing any callback storage. A failed close leaves the
     * supervisor, job identities and earlier successful releases available. */
    for (size_t index = 0U; index < supervisor->capacity; ++index) {
        status = UmiThreadRelease(&supervisor->jobs[index].thread);
        if (status != UMI_STATUS_OK) return status;
    }
    for (size_t index = 0U; index < supervisor->capacity; ++index)
        umi_cancellation_token_destroy(supervisor->jobs[index].cancellation);
    umi_condition_destroy(supervisor->condition);
    umi_mutex_destroy(supervisor->mutex);
    free(supervisor->jobs);
    free(supervisor);
    *inOutSupervisor = NULL;
    return UMI_STATUS_OK;
}

void umi_process_supervisor_destroy(UmiProcessSupervisor *supervisor)
{
    /* Keep the established void entry point. Checked owners use Release to
     * observe a failure and retain their pointer for a later attempt. */
    (void)UmiProcessSupervisorRelease(&supervisor);
}

UmiStatus UmiProcessSupervisorReleaseJob(UmiProcessSupervisor *supervisor,
    UmiProcessJobId jobId)
{
    UmiProcessJob *job;
    UmiStatus status;
    if (supervisor == NULL || jobId == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(supervisor->mutex);
    if (status != UMI_STATUS_OK) return status;
    job = find_job(supervisor, jobId);
    if (supervisor->shutting_down) status = UMI_STATUS_INVALID_STATE;
    else if (job == NULL) status = UMI_STATUS_NOT_FOUND;
    else if (job->state == UMI_PROCESS_JOB_CREATED ||
             job->state == UMI_PROCESS_JOB_RUNNING || job->waiters != 0U)
        status = UMI_STATUS_BUSY;
    else {
        status = SupervisorJoinJob(job);
        if (status == UMI_STATUS_OK) status = UmiThreadRelease(&job->thread);
        if (status == UMI_STATUS_OK) {
            umi_cancellation_token_destroy(job->cancellation);
            (void)memset(job, 0, sizeof(*job));
            --supervisor->count;
            supervisor->stats.jobs = supervisor->count;
        }
    }
    UmiStatus unlocked = umi_mutex_unlock(supervisor->mutex);
    return status != UMI_STATUS_OK ? status : unlocked;
}
