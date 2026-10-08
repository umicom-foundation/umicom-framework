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
#include "umicom/build/live_output.h"
#include "umicom/build/log_capture.h"
#include "umicom/build/job_history.h"
#include "umicom/platform/task_queue.h"
#include "umicom/platform/threading.h"
#include "umicom/platform/output_tail.h"

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
    /* Output belongs to the worker session, never to a frontend widget. */
    UmiBuildProjectExecuteObserved execute_observed;
    UmiBuildOutputSnapshot output;
    /* The worker owns file I/O; polling only reads this published copy. */
    UmiOutputFile *log_file;
    UmiBuildLogSnapshot log;
    /* The host owns the database; the session publishes only copied evidence. */
    UmiJobHistory *job_history;
    UmiBuildJobHistoryState durable;
};

/* All helpers below run with the session mutex held. Saturating counters
 * preserve monotonic evidence even if a very long-lived transport exhausts it. */
static void output_changed(UmiBuildOutputSnapshot *output)
{
    if (output->revision != UINT64_MAX) ++output->revision;
    else output->counters_saturated = true;
}

static void output_begin(UmiBuildProjectSession *session, size_t phase_index)
{
    UmiBuildOutputSnapshot *output = &session->output;
    uint64_t revision = output->revision;
    bool saturated = output->counters_saturated;
    memset(output, 0, sizeof(*output));
    output->operation_id = session->snapshot.operation_id;
    output->revision = revision;
    output->counters_saturated = saturated;
    output->phase_index = phase_index;
    output->phase = session->phases[phase_index];
    output->status = UMI_STATUS_OK;
    output->streamed = session->config.execute == NULL;
    output_changed(output);
}

/* Keep the newest bytes without allocations or unbounded growth in callbacks.
 * A chunk may itself exceed the capacity; only its tail needs to be copied. */
/* Bounded retention moves into the platform service so every worker uses the same
 * byte accounting and eviction rules. Build snapshot layout and behaviour stay compatible.
 * The previous implementation remains here for engineering review. */
#if 0
static void output_append(UmiBuildOutputSnapshot *output, const char *bytes, size_t length)
{
    const size_t limit = sizeof(output->bytes) - 1U;
    if (length == 0U) return;
    if (length > UINT64_MAX - output->total_bytes) {
        output->total_bytes = UINT64_MAX;
        output->counters_saturated = true;
    } else output->total_bytes += (uint64_t)length;
    if (length >= limit) {
        output->truncated = output->truncated || output->length != 0U || length > limit;
        memcpy(output->bytes, bytes + length - limit, limit);
        output->length = limit;
    } else {
        size_t available = limit - output->length;
        if (length > available) {
            size_t discarded = length - available;
            memmove(output->bytes, output->bytes + discarded, output->length - discarded);
            output->length -= discarded;
            output->truncated = true;
        }
        memcpy(output->bytes + output->length, bytes, length);
        output->length += length;
    }
    output->bytes[output->length] = '\0';
    output_changed(output);
}
#endif
static void output_append(UmiBuildOutputSnapshot *output, const char *bytes, size_t length)
{
    if (length == 0U) return;
    UmiOutputTailState tail = {
        .length = output->length, .total_bytes = output->total_bytes,
        .truncated = output->truncated, .counters_saturated = output->counters_saturated
    };
    /* The session lock protects both the borrowed storage and its counters.
     * Build and test workers now use the same eviction and overflow policy. */
    if (UmiOutputTailAppend(output->bytes, sizeof(output->bytes), &tail, bytes, length)
        != UMI_STATUS_OK) return;
    output->length = tail.length;
    output->total_bytes = tail.total_bytes;
    output->truncated = tail.truncated;
    output->counters_saturated = tail.counters_saturated;
    output_changed(output);
}

/* Perform disk I/O outside the session mutex, then publish small metadata.
 * This keeps frontend polling independent of a slow disk or storage failure. */
static void capture_log(UmiBuildProjectSession *session, const char *bytes, size_t length)
{
    if (session->log_file == NULL) return;
    UmiOutputFileSnapshot file;
    (void)UmiOutputFileWrite(session->log_file, bytes, length);
    (void)UmiOutputFileRead(session->log_file, &file);
    (void)umi_mutex_lock(session->mutex);
    if (length > UINT64_MAX - session->log.bytes_received) {
        session->log.bytes_received = UINT64_MAX;
        session->log.streamed = false; /* Exhausted evidence cannot claim completeness. */
    } else session->log.bytes_received += (uint64_t)length;
    session->log.file = file;
    (void)umi_mutex_unlock(session->mutex);
}

/* Closing belongs to the worker before it publishes overall completion. Even
 * cancellation or a failed command keeps the captured prefix for inspection. */
static void close_log(UmiBuildProjectSession *session)
{
    if (session->log_file == NULL) return;
    UmiOutputFileSnapshot file;
    (void)UmiOutputFileClose(session->log_file);
    (void)UmiOutputFileRead(session->log_file, &file);
    (void)umi_mutex_lock(session->mutex);
    session->log.file = file;
    session->log.capture_complete = session->log.streamed && file.closed &&
        file.status == UMI_STATUS_OK && file.bytes_written == session->log.bytes_received;
    (void)umi_mutex_unlock(session->mutex);
}

/* The worker copies borrowed process bytes; GTK stays on its owning thread. */
static void observe_output(const char *bytes, size_t length, void *context)
{
    UmiBuildProjectSession *session = context;
    if (bytes == NULL || length == 0U) return;
    (void)umi_mutex_lock(session->mutex);
    output_append(&session->output, bytes, length);
    (void)umi_mutex_unlock(session->mutex);
    capture_log(session, bytes, length);
}

/* Reuse the provider-neutral runner without sharing any mutable UI services. */
/* Worker output now flows through the observed runner or an explicit host
 * transport. Existing custom executors remain supported with final output.
 * The former implementation is retained for engineering review. */
#if 0
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
#endif
static UmiStatus run_phase(UmiBuildProjectSession *session, UmiBuildPhase phase,
    UmiBuildResult *result)
{
    UmiBuildRunner *runner = NULL;
    UmiBuildRunnerConfig config = {0};
    UmiStatus status;
    if (session->execute_observed != NULL)
        return session->execute_observed(&session->profile, phase,
            session->cancellation, observe_output, session, result, session->config.context);
    if (session->config.execute != NULL)
        return session->config.execute(&session->profile, phase,
            session->cancellation, result, session->config.context);
    config.profile = session->profile;
    config.clock = session->config.clock;
    config.cancellation = session->cancellation;
    status = umi_build_runner_create(&config, &runner);
    if (status == UMI_STATUS_OK) status = UmiBuildRunnerRunObserved(runner, phase, observe_output, session, result);
    umi_build_runner_destroy(runner);
    return status;
}


/* Disk transactions never run under the session mutex. Only this worker updates
 * a submitted job; frontend reads consume a small copy instead of waiting on I/O.
 * A failed checkpoint stops subsequent external actions. The prior database
 * record remains unfinished, so a later session cannot invent a final result. */
static UmiStatus record_job_progress(UmiBuildProjectSession *session,
    UmiJobHistoryState state, size_t completed, UmiStatus result)
{
    if (session->job_history==NULL) return UMI_STATUS_OK;
    UmiJobHistoryEntry current;
    UmiStatus prior;
    (void)umi_mutex_lock(session->mutex);
    current=session->durable.entry;
    prior=session->durable.storage_status;
    (void)umi_mutex_unlock(session->mutex);
    if (prior!=UMI_STATUS_OK) return prior;
    UmiJobHistoryEntry updated;
    UmiStatus status=UmiJobHistoryUpdate(session->job_history,current.id,current.revision,
        state,(unsigned)completed,result,&updated);
    (void)umi_mutex_lock(session->mutex);
    session->durable.storage_status=status;
    if (status==UMI_STATUS_OK) session->durable.entry=updated;
    (void)umi_mutex_unlock(session->mutex);
    return status;
}

/* Only this worker writes the current result slot. The mutex publishes it
 * after completion, so a polling frontend never sees a partially copied draft. */
/* The worker now checkpoints shared durable job evidence before starting work and between phases.
 * Completed phase output remains available if storage fails; no later phase is launched.
 * The former implementation is retained for engineering review. */
#if 0
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
        output_begin(session, index);
        (void)umi_mutex_unlock(session->mutex);
        umi_build_result_init(result, 0U, session->phases[index], session->profile.profile_id);
        status = run_phase(session, session->phases[index], result);
        result->operation_id = session->snapshot.operation_id * UINT64_C(8) + (uint64_t)index;
        /* Failed launch/executor results still expose the attempted phase. */
        if (result->state == UMI_BUILD_STATE_CREATED || result->state == UMI_BUILD_STATE_RUNNING)
            umi_build_result_finish(result, status, result->exit_code, result->duration_ms);
        /* Final-only custom executors remain useful but are never described
         * as complete streamed logs. Copy their bounded bytes exactly once. */
        if (session->config.execute != NULL) {
            size_t length = 0U;
            while (length < sizeof(result->output) && result->output[length] != '\0') ++length;
            capture_log(session, result->output, length);
        }
        (void)umi_mutex_lock(session->mutex);
        if (session->config.execute != NULL) {
            size_t length = 0U;
            while (length < sizeof(result->output) && result->output[length] != '\0') ++length;
            output_append(&session->output, result->output, length);
        }
        session->output.phase_complete = true;
        session->output.status = status;
        output_changed(&session->output);
        session->snapshot.completed_phase_count = index + 1U;
        session->snapshot.status = status;
        (void)umi_mutex_unlock(session->mutex);
        if (status != UMI_STATUS_OK) break;
    }
    close_log(session);
    (void)umi_mutex_lock(session->mutex);
    session->snapshot.status = status;
    session->snapshot.active = false;
    /* Cancellation before a phase starts still produces terminal evidence.
     * If an earlier phase completed successfully, keep its status intact:
     * cancelling the remaining workflow does not make that phase fail. */
    if (!session->output.phase_complete) {
        session->output.phase_complete = true;
        session->output.status = status;
        output_changed(&session->output);
    }
    (void)umi_mutex_unlock(session->mutex);
    return status;
}
#endif
static UmiStatus execute_session(UmiTaskContext *context, void *data)
{
    UmiBuildProjectSession *session = data;
    UmiStatus status = UMI_STATUS_OK;
    (void)context;
    size_t completed=0U;
    status=record_job_progress(session,UMI_JOB_HISTORY_RUNNING,0U,UMI_STATUS_OK);
    for (size_t index = 0U; status==UMI_STATUS_OK && index < session->phase_count; ++index) {
        UmiBuildResult *result = &session->results[index];
        if (umi_cancellation_token_is_requested(session->cancellation)) {
            status = UMI_STATUS_CANCELLED;
            break;
        }
        (void)umi_mutex_lock(session->mutex);
        session->snapshot.current_phase = session->phases[index];
        output_begin(session, index);
        (void)umi_mutex_unlock(session->mutex);
        umi_build_result_init(result, 0U, session->phases[index], session->profile.profile_id);
        status = run_phase(session, session->phases[index], result);
        result->operation_id = session->snapshot.operation_id * UINT64_C(8) + (uint64_t)index;
        /* Failed launch/executor results still expose the attempted phase. */
        if (result->state == UMI_BUILD_STATE_CREATED || result->state == UMI_BUILD_STATE_RUNNING)
            umi_build_result_finish(result, status, result->exit_code, result->duration_ms);
        /* Final-only custom executors remain useful but are never described
         * as complete streamed logs. Copy their bounded bytes exactly once. */
        if (session->config.execute != NULL) {
            size_t length = 0U;
            while (length < sizeof(result->output) && result->output[length] != '\0') ++length;
            capture_log(session, result->output, length);
        }
        (void)umi_mutex_lock(session->mutex);
        if (session->config.execute != NULL) {
            size_t length = 0U;
            while (length < sizeof(result->output) && result->output[length] != '\0') ++length;
            output_append(&session->output, result->output, length);
        }
        session->output.phase_complete = true;
        session->output.status = status;
        output_changed(&session->output);
        session->snapshot.completed_phase_count = index + 1U;
        session->snapshot.status = status;
        (void)umi_mutex_unlock(session->mutex);
        completed=index+1U;
        if (status != UMI_STATUS_OK) break;
        if (completed<session->phase_count)
            status=record_job_progress(session,UMI_JOB_HISTORY_RUNNING,completed,UMI_STATUS_OK);
    }
    UmiJobHistoryState outcome=status==UMI_STATUS_OK?UMI_JOB_HISTORY_SUCCEEDED:
        status==UMI_STATUS_CANCELLED?UMI_JOB_HISTORY_CANCELLED:UMI_JOB_HISTORY_FAILED;
    UmiStatus history_status=record_job_progress(session,outcome,completed,status);
    if (history_status!=UMI_STATUS_OK && status==UMI_STATUS_OK) status=history_status;
    close_log(session);
    (void)umi_mutex_lock(session->mutex);
    session->snapshot.status = status;
    session->snapshot.active = false;
    /* Cancellation before a phase starts still produces terminal evidence.
     * If an earlier phase completed successfully, keep its status intact:
     * cancelling the remaining workflow does not make that phase fail. */
    if (!session->output.phase_complete) {
        session->output.phase_complete = true;
        session->output.status = status;
        output_changed(&session->output);
    }
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
    UmiOutputFileDestroy(session->log_file);
    umi_task_destroy(session->task);
    umi_cancellation_token_destroy(session->cancellation);
    umi_mutex_destroy(session->mutex);
    free(session->results);
    free(session);
}

/* Accept only one job; queue idleness also covers the task's final bookkeeping
 * after the worker publishes completion, preventing early task destruction. */
/* The shared submission path now optionally reserves a new capture file before
 * queuing work. Trust, phase planning and existing unlogged behaviour remain.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_build_project_session_submit(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, UmiBuildPhase phase, bool trusted)
{
    UmiTaskQueueStats stats;
    UmiTaskConfig task_config = {0};
    UmiStatus status;
    if (session == NULL || profile == NULL || phase < UMI_BUILD_PHASE_CONFIGURE ||
        phase > UMI_BUILD_PHASE_DEPLOY) return UMI_STATUS_INVALID_ARGUMENT;
    if (!trusted) return UMI_STATUS_PERMISSION_DENIED;
    if ((phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY) &&
        !profile->build_testing) return UMI_STATUS_INVALID_STATE;
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
/* Previous three-stage plan retained. The same worker now gates delivery on tests. */
//     if (phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL)
//         session->profile.build_target[0] = '\0';
//     session->phase_count = 0U;
//     if (phase == UMI_BUILD_PHASE_BUILD || phase == UMI_BUILD_PHASE_RUN ||
//         phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL)
//         session->phases[session->phase_count++] = UMI_BUILD_PHASE_CONFIGURE;
//     if (phase == UMI_BUILD_PHASE_RUN || phase == UMI_BUILD_PHASE_TEST ||
//         phase == UMI_BUILD_PHASE_INSTALL)
//         session->phases[session->phase_count++] = UMI_BUILD_PHASE_BUILD;
//     session->phases[session->phase_count++] = phase;
    if (phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL ||
        phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY)
        session->profile.build_target[0] = '\0';
    session->phase_count = 0U;
    if (phase == UMI_BUILD_PHASE_REBUILD) {
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_CONFIGURE;
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_CLEAN;
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_BUILD;
    } else {
        if (phase == UMI_BUILD_PHASE_BUILD || phase == UMI_BUILD_PHASE_RUN ||
            phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL ||
            phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY)
            session->phases[session->phase_count++] = UMI_BUILD_PHASE_CONFIGURE;
        if (phase == UMI_BUILD_PHASE_RUN || phase == UMI_BUILD_PHASE_TEST ||
            phase == UMI_BUILD_PHASE_INSTALL || phase == UMI_BUILD_PHASE_PACKAGE ||
            phase == UMI_BUILD_PHASE_DEPLOY)
            session->phases[session->phase_count++] = UMI_BUILD_PHASE_BUILD;
        if (phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY)
            session->phases[session->phase_count++] = UMI_BUILD_PHASE_TEST;
        session->phases[session->phase_count++] = phase;
    }
    umi_cancellation_token_reset(session->cancellation);
    (void)umi_mutex_lock(session->mutex);
    ++session->snapshot.operation_id;
    session->snapshot.requested_phase = phase;
    session->snapshot.current_phase = session->phases[0];
    session->snapshot.completed_phase_count = 0U;
    session->snapshot.status = UMI_STATUS_OK;
    session->snapshot.active = true;
    memset(&session->output, 0, sizeof(session->output));
    output_begin(session, 0U);
    (void)umi_mutex_unlock(session->mutex);
    status = umi_task_queue_submit(session->queue, session->task);
    if (status != UMI_STATUS_OK) {
        (void)umi_mutex_lock(session->mutex);
        session->snapshot.active = false; session->snapshot.status = status;
        session->output.phase_complete = true;
        session->output.status = status;
        output_changed(&session->output);
        (void)umi_mutex_unlock(session->mutex);
    }
    return status;
}
#endif
/* Submission now reserves shared job evidence before it opens a log or starts a worker.
 * The same phase planner, trust checks and exclusive log rules remain in the replacement.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiBuildProjectSessionSubmitLogged(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, UmiBuildPhase phase, bool trusted, const char *log_path)
{
    UmiTaskQueueStats stats;
    UmiTaskConfig task_config = {0};
    UmiStatus status;
    if (session == NULL || profile == NULL || phase < UMI_BUILD_PHASE_CONFIGURE ||
        phase > UMI_BUILD_PHASE_DEPLOY) return UMI_STATUS_INVALID_ARGUMENT;
    if (!trusted) return UMI_STATUS_PERMISSION_DENIED;
    if ((phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY) &&
        !profile->build_testing) return UMI_STATUS_INVALID_STATE;
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
    UmiOutputFile *new_log = NULL;
    if (log_path != NULL) {
        status = UmiOutputFileCreate(log_path, &new_log);
        if (status != UMI_STATUS_OK) return status;
    }
    /* Old log files remain on disk; only their closed handle is released. */
    UmiOutputFileDestroy(session->log_file);
    session->log_file = new_log;
    session->profile = *profile;
/* Previous three-stage plan retained. The same worker now gates delivery on tests. */
//     if (phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL)
//         session->profile.build_target[0] = '\0';
//     session->phase_count = 0U;
//     if (phase == UMI_BUILD_PHASE_BUILD || phase == UMI_BUILD_PHASE_RUN ||
//         phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL)
//         session->phases[session->phase_count++] = UMI_BUILD_PHASE_CONFIGURE;
//     if (phase == UMI_BUILD_PHASE_RUN || phase == UMI_BUILD_PHASE_TEST ||
//         phase == UMI_BUILD_PHASE_INSTALL)
//         session->phases[session->phase_count++] = UMI_BUILD_PHASE_BUILD;
//     session->phases[session->phase_count++] = phase;
    if (phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL ||
        phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY)
        session->profile.build_target[0] = '\0';
    session->phase_count = 0U;
    if (phase == UMI_BUILD_PHASE_REBUILD) {
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_CONFIGURE;
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_CLEAN;
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_BUILD;
    } else {
        if (phase == UMI_BUILD_PHASE_BUILD || phase == UMI_BUILD_PHASE_RUN ||
            phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL ||
            phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY)
            session->phases[session->phase_count++] = UMI_BUILD_PHASE_CONFIGURE;
        if (phase == UMI_BUILD_PHASE_RUN || phase == UMI_BUILD_PHASE_TEST ||
            phase == UMI_BUILD_PHASE_INSTALL || phase == UMI_BUILD_PHASE_PACKAGE ||
            phase == UMI_BUILD_PHASE_DEPLOY)
            session->phases[session->phase_count++] = UMI_BUILD_PHASE_BUILD;
        if (phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY)
            session->phases[session->phase_count++] = UMI_BUILD_PHASE_TEST;
        session->phases[session->phase_count++] = phase;
    }
    umi_cancellation_token_reset(session->cancellation);
    (void)umi_mutex_lock(session->mutex);
    ++session->snapshot.operation_id;
    memset(&session->log, 0, sizeof(session->log));
    session->log.operation_id = session->snapshot.operation_id;
    session->log.enabled = new_log != NULL;
    session->log.streamed = session->config.execute == NULL;
    if (new_log != NULL) (void)UmiOutputFileRead(new_log, &session->log.file);
    session->snapshot.requested_phase = phase;
    session->snapshot.current_phase = session->phases[0];
    session->snapshot.completed_phase_count = 0U;
    session->snapshot.status = UMI_STATUS_OK;
    session->snapshot.active = true;
    memset(&session->output, 0, sizeof(session->output));
    output_begin(session, 0U);
    (void)umi_mutex_unlock(session->mutex);
    status = umi_task_queue_submit(session->queue, session->task);
    if (status != UMI_STATUS_OK) {
        close_log(session);
        (void)umi_mutex_lock(session->mutex);
        session->log.capture_complete = false; /* No worker was accepted. */
        session->snapshot.active = false; session->snapshot.status = status;
        session->output.phase_complete = true;
        session->output.status = status;
        output_changed(&session->output);
        (void)umi_mutex_unlock(session->mutex);
    }
    return status;
}
#endif
UmiStatus UmiBuildProjectSessionSubmitLogged(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, UmiBuildPhase phase, bool trusted, const char *log_path)
{
    UmiTaskQueueStats stats;
    UmiTaskConfig task_config = {0};
    UmiStatus status;
    if (session == NULL || profile == NULL || phase < UMI_BUILD_PHASE_CONFIGURE ||
        phase > UMI_BUILD_PHASE_DEPLOY) return UMI_STATUS_INVALID_ARGUMENT;
    if (!trusted) return UMI_STATUS_PERMISSION_DENIED;
    if ((phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY) &&
        !profile->build_testing) return UMI_STATUS_INVALID_STATE;
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
/* Previous three-stage plan retained. The same worker now gates delivery on tests. */
//     if (phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL)
//         session->profile.build_target[0] = '\0';
//     session->phase_count = 0U;
//     if (phase == UMI_BUILD_PHASE_BUILD || phase == UMI_BUILD_PHASE_RUN ||
//         phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL)
//         session->phases[session->phase_count++] = UMI_BUILD_PHASE_CONFIGURE;
//     if (phase == UMI_BUILD_PHASE_RUN || phase == UMI_BUILD_PHASE_TEST ||
//         phase == UMI_BUILD_PHASE_INSTALL)
//         session->phases[session->phase_count++] = UMI_BUILD_PHASE_BUILD;
//     session->phases[session->phase_count++] = phase;
    if (phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL ||
        phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY)
        session->profile.build_target[0] = '\0';
    session->phase_count = 0U;
    if (phase == UMI_BUILD_PHASE_REBUILD) {
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_CONFIGURE;
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_CLEAN;
        session->phases[session->phase_count++] = UMI_BUILD_PHASE_BUILD;
    } else {
        if (phase == UMI_BUILD_PHASE_BUILD || phase == UMI_BUILD_PHASE_RUN ||
            phase == UMI_BUILD_PHASE_TEST || phase == UMI_BUILD_PHASE_INSTALL ||
            phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY)
            session->phases[session->phase_count++] = UMI_BUILD_PHASE_CONFIGURE;
        if (phase == UMI_BUILD_PHASE_RUN || phase == UMI_BUILD_PHASE_TEST ||
            phase == UMI_BUILD_PHASE_INSTALL || phase == UMI_BUILD_PHASE_PACKAGE ||
            phase == UMI_BUILD_PHASE_DEPLOY)
            session->phases[session->phase_count++] = UMI_BUILD_PHASE_BUILD;
        if (phase == UMI_BUILD_PHASE_PACKAGE || phase == UMI_BUILD_PHASE_DEPLOY)
            session->phases[session->phase_count++] = UMI_BUILD_PHASE_TEST;
        session->phases[session->phase_count++] = phase;
    }
    UmiJobHistoryEntry prepared={0};
    if (session->job_history!=NULL) {
        status=UmiJobHistoryBegin(session->job_history,"build.workflow",
            umi_build_phase_text(phase),(unsigned)session->phase_count,&prepared);
        (void)umi_mutex_lock(session->mutex);
        session->durable.enabled=true;
        session->durable.storage_status=status;
        session->durable.entry=prepared;
        (void)umi_mutex_unlock(session->mutex);
        if (status!=UMI_STATUS_OK) return status;
    }
    UmiOutputFile *new_log=NULL;
    if (log_path!=NULL) {
        status=UmiOutputFileCreate(log_path,&new_log);
        if (status!=UMI_STATUS_OK) {
            (void)record_job_progress(session,UMI_JOB_HISTORY_FAILED,0U,status);
            return status;
        }
    }
    /* Preserve prior log files; release only the previous closed handle. */
    UmiOutputFileDestroy(session->log_file);
    session->log_file=new_log;
    umi_cancellation_token_reset(session->cancellation);
    (void)umi_mutex_lock(session->mutex);
    ++session->snapshot.operation_id;
    memset(&session->log, 0, sizeof(session->log));
    session->log.operation_id = session->snapshot.operation_id;
    session->log.enabled = new_log != NULL;
    session->log.streamed = session->config.execute == NULL;
    if (new_log != NULL) (void)UmiOutputFileRead(new_log, &session->log.file);
    session->snapshot.requested_phase = phase;
    session->snapshot.current_phase = session->phases[0];
    session->snapshot.completed_phase_count = 0U;
    session->snapshot.status = UMI_STATUS_OK;
    session->snapshot.active = true;
    memset(&session->output, 0, sizeof(session->output));
    output_begin(session, 0U);
    (void)umi_mutex_unlock(session->mutex);
    status = umi_task_queue_submit(session->queue, session->task);
    if (status != UMI_STATUS_OK) {
        (void)record_job_progress(session,UMI_JOB_HISTORY_FAILED,0U,status);
        close_log(session);
        (void)umi_mutex_lock(session->mutex);
        session->log.capture_complete = false; /* No worker was accepted. */
        session->snapshot.active = false; session->snapshot.status = status;
        session->output.phase_complete = true;
        session->output.status = status;
        output_changed(&session->output);
        (void)umi_mutex_unlock(session->mutex);
    }
    return status;
}
/* Preserve the unlogged API through the same submission and phase planner. */
UmiStatus umi_build_project_session_submit(UmiBuildProjectSession *session,
    const UmiBuildProfile *profile, UmiBuildPhase phase, bool trusted)
{
    return UmiBuildProjectSessionSubmitLogged(session, profile, phase, trusted, NULL);
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

// MIGRATION REFERENCE — previous implementation excerpts
// The shared build session now distinguishes Package, Rebuild and local Deploy. Command execution stays in src/build/runner.c and its existing providers; CPack uses src/build/cpack_provider.c.
// These comments explain superseded statements; do not enable both execution paths.
// Previous source near line 134:
//         phase > UMI_BUILD_PHASE_INSTALL) return UMI_STATUS_INVALID_ARGUMENT;

/* This additive constructor preserves the existing configuration layout and
 * installs the alternate transport before any work can be submitted. */
UmiStatus UmiBuildProjectSessionCreateObserved(const UmiBuildProjectSessionConfig *config,
    UmiBuildProjectExecuteObserved execute, UmiBuildProjectSession **out_session)
{
    if (out_session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_session = NULL;
    if (config == NULL || (execute != NULL && config->execute != NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_build_project_session_create(config, out_session);
    if (status == UMI_STATUS_OK) (*out_session)->execute_observed = execute;
    return status;
}

/* Copy under the same lock as publication; no borrowed worker buffer escapes. */
UmiStatus UmiBuildProjectSessionReadOutput(UmiBuildProjectSession *session,
    UmiBuildOutputSnapshot *out_snapshot)
{
    if (session == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(session->mutex);
    *out_snapshot = session->output;
    (void)umi_mutex_unlock(session->mutex);
    return UMI_STATUS_OK;
}

/* Poll the worker's published metadata rather than reading its native handle. */
UmiStatus UmiBuildProjectSessionReadLog(UmiBuildProjectSession *session, UmiBuildLogSnapshot *out_snapshot)
{
    if (session == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(session->mutex);
    *out_snapshot = session->log;
    (void)umi_mutex_unlock(session->mutex);
    return UMI_STATUS_OK;
}

/* Binding is an owner-thread operation. Worker completion is not sufficient
 * until the queue has also released its task reference. */
UmiStatus UmiBuildProjectSessionSetHistory(UmiBuildProjectSession *session,UmiJobHistory *history)
{
    if (session==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTaskQueueStats stats=umi_task_queue_stats(session->queue);
    if (stats.queued!=0 || stats.running!=0) return UMI_STATUS_BUSY;
    (void)umi_mutex_lock(session->mutex);
    session->job_history=history;
    memset(&session->durable,0,sizeof(session->durable));
    session->durable.enabled=history!=NULL;
    (void)umi_mutex_unlock(session->mutex);
    return UMI_STATUS_OK;
}
UmiStatus UmiBuildProjectSessionReadHistory(UmiBuildProjectSession *session,UmiBuildJobHistoryState *out_state)
{
    if (session==NULL || out_state==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)umi_mutex_lock(session->mutex);
    *out_state=session->durable;
    (void)umi_mutex_unlock(session->mutex);
    return UMI_STATUS_OK;
}
