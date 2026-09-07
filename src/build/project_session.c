/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/build/project_session.c
 *
 * PURPOSE:
 *   Adapt the existing build runner to cancellable, bounded worker execution.
 *   UI history and document services stay on their original owning thread.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/project_session.h"
#include "umicom/platform/task_queue.h"
#include "umicom/platform/threading.h"

#include <stdlib.h>
#include <string.h>

struct UmiBuildProjectSession {
    UmiBuildProjectSessionConfig config;
    UmiTaskQueue *queue;
    UmiTask *task;
    UmiMutex *mutex;
    UmiCancellationToken *cancellation;
    UmiBuildProfile profile;
    UmiBuildPhase phases[UMI_BUILD_PROJECT_SESSION_MAX_PHASES];
    size_t phase_count;
    UmiBuildResult *results;
    UmiBuildProjectSessionSnapshot snapshot;
};

/* Reuse the provider-neutral runner without sharing any mutable UI services. */
static UmiStatus run_phase(UmiBuildProjectSession *session, UmiBuildPhase phase,
    UmiBuildResult *result)
{
    UmiBuildRunner *runner = NULL;
    UmiBuildRunnerConfig config = {0};
    UmiStatus status;
    if (session->config.execute != NULL)
        return session->config.execute(&session->profile, phase,
            session->cancellation, result, session->config.context);
    config.profile = session->profile;
    config.clock = session->config.clock;
    config.cancellation = session->cancellation;
    status = umi_build_runner_create(&config, &runner);
    if (status == UMI_STATUS_OK) status = umi_build_runner_run(runner, phase, result);
    umi_build_runner_destroy(runner);
    return status;
}

/* Only this worker writes the current result slot. The mutex publishes it
 * after completion, so a polling frontend never sees a partially copied draft. */
static UmiStatus execute_session(UmiTaskContext *context, void *data)
{
    UmiBuildProjectSession *session = data;
    UmiStatus status = UMI_STATUS_OK;
    (void)context;
    for (size_t index = 0U; index < session->phase_count; ++index) {
        UmiBuildResult *result = &session->results[index];
        if (umi_cancellation_token_is_requested(session->cancellation)) {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        (void)umi_mutex_lock(session->mutex);
        session->snapshot.current_phase = session->phases[index];
        (void)umi_mutex_unlock(session->mutex);
        umi_build_result_init(result, 0U, session->phases[index], session->profile.profile_id);
        status = run_phase(session, session->phases[index], result);
        result->operation_id = session->snapshot.operation_id * UINT64_C(8) + (uint64_t)index;
        /* Failed launch/executor results still expose the attempted phase. */
        if (result->state == UMI_BUILD_STATE_CREATED || result->state == UMI_BUILD_STATE_RUNNING)
            umi_build_result_finish(result, status, result->exit_code, result->duration_ms);
        (void)umi_mutex_lock(session->mutex);
        session->snapshot.completed_phase_count = index + 1U;
        session->snapshot.status = status;
        (void)umi_mutex_unlock(session->mutex);
        if (status != UMI_STATUS_OK) break;
    }
    (void)umi_mutex_lock(session->mutex);
    session->snapshot.status = status;
    session->snapshot.active = false;
    (void)umi_mutex_unlock(session->mutex);
    return status;
}

/* Allocate bounded result storage on the heap, never a large UI/worker stack. */
UmiStatus umi_build_project_session_create(const UmiBuildProjectSessionConfig *config,
    UmiBuildProjectSession **out_session)
{
    UmiBuildProjectSession *session;
    UmiTaskQueueConfig queue_config = {1U, 1U};
    UmiStatus status;
    if (config == NULL || out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_session = NULL;
    session = calloc(1U, sizeof(*session));
    if (session == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    session->config = *config;
    session->results = calloc(UMI_BUILD_PROJECT_SESSION_MAX_PHASES, sizeof(*session->results));
    status = session->results != NULL ? umi_mutex_create(&session->mutex) : UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK) status = umi_cancellation_token_create(&session->cancellation);
    if (status == UMI_STATUS_OK) status = umi_task_queue_create(&queue_config, &session->queue);
    if (status != UMI_STATUS_OK) { umi_build_project_session_destroy(session); return status; }
    *out_session = session;
    return UMI_STATUS_OK;
}

/* Join before freeing the task, profile, cancellation token or result buffers. */
void umi_build_project_session_destroy(UmiBuildProjectSession *session)
{
    if (session == NULL) return;
    umi_build_project_session_cancel(session);
    if (session->queue != NULL) (void)umi_task_queue_shutdown(session->queue, 0);
    umi_task_queue_destroy(session->queue);
    umi_task_destroy(session->task);
    umi_cancellation_token_destroy(session->cancellation);
    umi_mutex_destroy(session->mutex);
    free(session->results);
    free(session);
}

/* Accept only one job; queue idleness also covers the task's final bookkeeping
 * after the worker publishes completion, preventing early task destruction. */
UmiStatus umi_build_project_session_submit(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, UmiBuildPhase phase, bool trusted)
{
    UmiTaskQueueStats stats;
    UmiTaskConfig task_config = {0};
    UmiStatus status;
    if (session == NULL || profile == NULL || phase < UMI_BUILD_PHASE_CONFIGURE ||
        phase > UMI_BUILD_PHASE_INSTALL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!trusted) return UMI_STATUS_PERMISSION_DENIED;
    status = umi_build_profile_validate(profile, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;
    if (phase == UMI_BUILD_PHASE_RUN && profile->run_program[0] == '\0') return UMI_STATUS_INVALID_STATE;
    stats = umi_task_queue_stats(session->queue);
    if (stats.queued != 0U || stats.running != 0U) return UMI_STATUS_BUSY;
    if (session->snapshot.operation_id >= UINT64_MAX / UINT64_C(8) - 1U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_task_destroy(session->task); session->task = NULL;
    task_config.label = "Project build workflow";
    task_config.function = execute_session; task_config.user_data = session;
    status = umi_task_create(&task_config, &session->task);
    if (status != UMI_STATUS_OK) return status;
    session->profile = *profile;
    session->phase_count = 0U;
    if (phase == UMI_BUILD_PHASE_BUILD || phase == UMI_BUILD_PHASE_RUN)
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_CONFIGURE;
    if (phase == UMI_BUILD_PHASE_RUN)
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_BUILD;
    session->phases[session->phase_count++] = phase;
    umi_cancellation_token_reset(session->cancellation);
    (void)umi_mutex_lock(session->mutex);
    ++session->snapshot.operation_id;
    session->snapshot.requested_phase = phase;
    session->snapshot.current_phase = session->phases[0];
    session->snapshot.completed_phase_count = 0U;
    session->snapshot.status = UMI_STATUS_OK;
    session->snapshot.active = true;
    (void)umi_mutex_unlock(session->mutex);
    status = umi_task_queue_submit(session->queue, session->task);
    if (status != UMI_STATUS_OK) {
        (void)umi_mutex_lock(session->mutex);
        session->snapshot.active = false; session->snapshot.status = status;
        (void)umi_mutex_unlock(session->mutex);
    }
    return status;
}

/* Return stable evidence while accounting for the queue's final task access. */
UmiStatus umi_build_project_session_snapshot(UmiBuildProjectSession *session,
    UmiBuildProjectSessionSnapshot *out_snapshot)
{
    UmiTaskQueueStats stats;
    if (session == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(session->mutex);
    *out_snapshot = session->snapshot;
    (void)umi_mutex_unlock(session->mutex);
    stats = umi_task_queue_stats(session->queue);
    out_snapshot->active = out_snapshot->active || stats.queued != 0U || stats.running != 0U;
    out_snapshot->cancellation_requested = umi_cancellation_token_is_requested(session->cancellation) != 0;
    return UMI_STATUS_OK;
}

/* Copy only fully published phase output; the caller retains its own buffer. */
UmiStatus umi_build_project_session_result_at(UmiBuildProjectSession *session,
    size_t index, UmiBuildResult *out_result)
{
    if (session == NULL || out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(session->mutex);
    if (index >= session->snapshot.completed_phase_count) {
        (void)umi_mutex_unlock(session->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_result = session->results[index];
    (void)umi_mutex_unlock(session->mutex);
    return UMI_STATUS_OK;
}

/* Cancel the process token without cancelling away queued completion evidence. */
void umi_build_project_session_cancel(UmiBuildProjectSession *session)
{
    if (session != NULL && session->cancellation != NULL)
        umi_cancellation_token_request(session->cancellation);
}
