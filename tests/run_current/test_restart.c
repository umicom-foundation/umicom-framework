/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_current/test_restart.c
 *
 * PURPOSE:
 *   Exercise restart admission, terminal handover, cancellation revocation and retained evidence with a controlled executor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/build/run_restart.h"
#include "umicom/build/log_capture.h"
#include "umicom/build/job_history.h"
#include <stdatomic.h>

typedef struct RestartProbe {
    atomic_int calls, stage, permit, release_cancel;
    UmiBuildPhase wait_phase;
} RestartProbe;
static UmiStatus RestartExecute(const UmiBuildProfile *profile, UmiBuildPhase phase,
    UmiCancellationToken *token, UmiBuildResult *result, void *context)
{
    (void)profile;
    RestartProbe *probe = context;
    atomic_fetch_add(&probe->calls, 1);
    atomic_store(&probe->stage, (int)phase);
    if (phase == probe->wait_phase) {
        while (!atomic_load(&probe->permit) && !umi_cancellation_token_is_requested(token))
            umi_thread_sleep_ms(1U);
        if (umi_cancellation_token_is_requested(token)) {
            /* A deterministic gap proves that requesting stop cannot itself
             * authorize another worker before the old executor returns. */
            while (!atomic_load(&probe->release_cancel)) umi_thread_sleep_ms(1U);
            strcpy(result->output, "first program stopped\n");
            return UMI_STATUS_CANCELLED;
        }
    }
    strcpy(result->output, "program completed\n");
    return UMI_STATUS_OK;
}
static int StopFromAnotherThread(void *session)
{
    umi_build_project_session_cancel(session);
    return 0;
}
static void WaitStage(RestartProbe *probe, UmiBuildPhase phase)
{
    for (unsigned i = 0U; i < 5000U && atomic_load(&probe->stage) != (int)phase; ++i)
        umi_thread_sleep_ms(1U);
    CHECK(atomic_load(&probe->stage) == (int)phase);
}
static void CheckFileText(const char *path, const char *expected)
{
    char *text = NULL;
    size_t length = 0U;
    CHECK(umi_fs_read_text(path, &text, &length) == UMI_STATUS_OK);
    CHECK(strcmp(text, expected) == 0);
    umi_fs_free_text(text);
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    RestartProbe probe = {0};
    /* Configure is enum value zero; use a distinct unpublished-stage sentinel. */
    atomic_store(&probe.stage, -1);
    atomic_store(&probe.release_cancel, 1);
    probe.wait_phase = UMI_BUILD_PHASE_RUN;
    UmiBuildProfile *profile = malloc(sizeof *profile);
    CHECK(profile != NULL);
    FixtureProfile(profile);
    char root[UMI_PATH_CAPACITY], other[UMI_PATH_CAPACITY], log[UMI_PATH_CAPACITY];
    strcpy(root, profile->source_directory);
    CHECK(umi_path_join(root, "other", other, sizeof other) == UMI_STATUS_OK);
    CHECK(umi_fs_make_directories(other) == UMI_STATUS_OK);
    CHECK(umi_path_join(root, "restart.log", log, sizeof log) == UMI_STATUS_OK);
    (void)umi_fs_remove_tree(log);
    char fresh[UMI_PATH_CAPACITY];
    CHECK(snprintf(fresh, sizeof fresh, "%s.restart-2", log) > 0);
    (void)umi_fs_remove_tree(fresh);
    UmiBuildProjectSessionConfig config = {NULL, RestartExecute, &probe};
    UmiBuildProjectSession *session = NULL;
    CHECK(umi_build_project_session_create(&config, &session) == UMI_STATUS_OK);
    UmiDataServer *database = NULL;
    UmiJobHistory *history = NULL;
    CHECK(umi_data_server_create_memory(&database) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCreate(database, "restart", &history) == UMI_STATUS_OK);
    CHECK(UmiBuildProjectSessionSetHistory(session, history) == UMI_STATUS_OK);
    bool submitted = true;
    UmiBuildRunRestartSnapshot intent;
    if (!strcmp(name, "invalid")) {
        CHECK(UmiBuildRunRestartRead(NULL, &intent) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildRunRestartRead(session, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildRunRestartRequest(NULL, profile, root, true) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildRunRestartAdvance(NULL, profile, root, true, 0U, &submitted) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!submitted);
        CHECK(UmiBuildRunRestartAdvance(session, profile, root, true, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(name, "idle")) {
        CHECK(UmiBuildRunRestartReview(session, profile, root, true) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiBuildRunRestartRequest(session, profile, root, true) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiBuildRunRestartAdvance(session, profile, root, true, 0U, &submitted) == UMI_STATUS_NOT_FOUND && !submitted);
    } else {
        bool rejected_phase = !strcmp(name, "configure") || !strcmp(name, "build") ||
            !strcmp(name, "test") || !strcmp(name, "run-configure") || !strcmp(name, "run-build");
        UmiBuildPhase request = UMI_BUILD_PHASE_RUN;
        if (!strcmp(name, "configure") || !strcmp(name, "run-configure"))
            probe.wait_phase = UMI_BUILD_PHASE_CONFIGURE;
        if (!strcmp(name, "build") || !strcmp(name, "run-build"))
            probe.wait_phase = UMI_BUILD_PHASE_BUILD;
        if (!strcmp(name, "test")) probe.wait_phase = UMI_BUILD_PHASE_TEST;
        if (rejected_phase && strncmp(name, "run-", 4U)) request = probe.wait_phase;
        bool logged = !strcmp(name, "log") || !strcmp(name, "log-collision");
        CHECK((rejected_phase ? UmiBuildProjectSessionSubmitLogged(session, profile, request, true, NULL) :
            UmiBuildProjectSessionRunCurrent(session, profile, true, logged ? log : NULL)) == UMI_STATUS_OK);
        WaitStage(&probe, probe.wait_phase);
        UmiBuildProjectSessionSnapshot before;
        CHECK(umi_build_project_session_snapshot(session, &before) == UMI_STATUS_OK);
        CHECK(before.operation_id == 1U && before.active);
        if (rejected_phase || !strcmp(name, "untrusted") || !strcmp(name, "other-project") ||
            !strcmp(name, "changed-profile")) {
            if (!strcmp(name, "changed-profile")) strcpy(profile->run_arguments, "different");
            UmiStatus refusal = UmiBuildRunRestartRequest(session, profile,
                !strcmp(name, "other-project") ? other : root, strcmp(name, "untrusted") != 0);
            CHECK(refusal != UMI_STATUS_OK);
            CHECK(umi_build_project_session_snapshot(session, &before) == UMI_STATUS_OK &&
                !before.cancellation_requested);
            CHECK(UmiBuildRunRestartRead(session, &intent) == UMI_STATUS_OK && !intent.pending);
            umi_build_project_session_cancel(session);
            (void)FixtureWait(session);
        } else {
            bool hold = !strcmp(name, "waiting");
            if (hold) atomic_store(&probe.release_cancel, 0);
            CHECK(UmiBuildRunRestartReview(session, profile, root, true) == UMI_STATUS_OK);
            CHECK(UmiBuildRunRestartRequest(session, profile, root, true) == UMI_STATUS_OK);
            CHECK(UmiBuildRunRestartRead(session, &intent) == UMI_STATUS_OK && intent.pending &&
                intent.previous_operation_id == 1U && intent.submitted_operation_id == 0U);
            if (!strcmp(name, "duplicate"))
                CHECK(UmiBuildRunRestartRequest(session, profile, root, true) == UMI_STATUS_BUSY);
            if (!strcmp(name, "pending-submit"))
                CHECK(UmiBuildProjectSessionRunCurrent(session, profile, true, NULL) == UMI_STATUS_BUSY);
            if (hold) {
                CHECK(UmiBuildRunRestartAdvance(session, profile, root, true, 0U, &submitted) == UMI_STATUS_BUSY && !submitted);
                CHECK(atomic_load(&probe.calls) == 1);
                atomic_store(&probe.release_cancel, 1);
            }
            if (!strcmp(name, "destroy")) {
                umi_build_project_session_destroy(session);
                session = NULL;
                CHECK(atomic_load(&probe.calls) == 1);
            } else {
                UmiBuildProjectSessionSnapshot stopped = FixtureWait(session);
                CHECK(stopped.status == UMI_STATUS_CANCELLED && stopped.completed_phase_count == 1U);
                if (!strcmp(name, "stop") || !strcmp(name, "thread-stop")) {
                    if (!strcmp(name, "thread-stop")) {
                        UmiThread *thread = NULL;
                        CHECK(umi_thread_start(StopFromAnotherThread, session, &thread) == UMI_STATUS_OK);
                        int exit_code = -1;
                        CHECK(umi_thread_join(thread, &exit_code) == UMI_STATUS_OK && exit_code == 0);
                        umi_thread_destroy(thread);
                    } else umi_build_project_session_cancel(session);
                    CHECK(UmiBuildRunRestartAdvance(session, profile, root, true, 1U, &submitted) == UMI_STATUS_NOT_FOUND && !submitted);
                    CHECK(UmiBuildRunRestartRead(session, &intent) == UMI_STATUS_OK &&
                        !intent.pending && intent.status == UMI_STATUS_CANCELLED);
                } else {
                    if (!strcmp(name, "handover"))
                        CHECK(UmiBuildRunRestartAdvance(session, profile, root, true, 0U, &submitted) == UMI_STATUS_BUSY && !submitted);
                    UmiBuildResult *result = malloc(sizeof *result);
                    CHECK(result != NULL);
                    CHECK(umi_build_project_session_result_at(session, 0U, result) == UMI_STATUS_OK);
                    CHECK(result->status == UMI_STATUS_CANCELLED);
                    free(result);
                    UmiBuildJobHistoryState previous;
                    CHECK(UmiBuildProjectSessionReadHistory(session, &previous) == UMI_STATUS_OK &&
                        previous.entry.state == UMI_JOB_HISTORY_CANCELLED);
                    if (logged) {
                        CHECK(strcmp(intent.log_path, fresh) == 0);
                        CheckFileText(log, "first program stopped\n");
                    }
                    if (!strcmp(name, "log-collision"))
                        CHECK(umi_fs_write_text(fresh, "keep this existing file\n") == UMI_STATUS_OK);
                    if (!strcmp(name, "changed-after")) strcpy(profile->run_arguments, "changed after request");
                    if (!strcmp(name, "missing-program")) CHECK(umi_fs_remove_tree("selected-program") == UMI_STATUS_OK);
                    bool refused = !strcmp(name, "revoked") || !strcmp(name, "changed-after") ||
                        !strcmp(name, "other-project-after") || !strcmp(name, "bad-count") ||
                        !strcmp(name, "missing-program") || !strcmp(name, "log-collision");
                    atomic_store(&probe.permit, 1);
                    UmiStatus advanced = UmiBuildRunRestartAdvance(session, profile,
                        !strcmp(name, "other-project-after") ? other : root,
                        strcmp(name, "revoked") != 0, !strcmp(name, "bad-count") ? 2U : 1U, &submitted);
                    CHECK((advanced == UMI_STATUS_OK) == !refused && submitted == !refused);
                    CHECK(UmiBuildRunRestartRead(session, &intent) == UMI_STATUS_OK && !intent.pending);
                    if (refused) {
                        CHECK(atomic_load(&probe.calls) == 1 && intent.submitted_operation_id == 0U);
                        if (!strcmp(name, "log-collision")) CheckFileText(fresh, "keep this existing file\n");
                    } else {
                        CHECK(!strcmp(name, "success") || !strcmp(name, "waiting") || !strcmp(name, "handover") ||
                            !strcmp(name, "duplicate") || !strcmp(name, "pending-submit") || !strcmp(name, "log"));
                        UmiBuildProjectSessionSnapshot complete = FixtureWait(session);
                        CHECK(complete.operation_id == 2U && complete.completed_phase_count == 1U &&
                            complete.current_phase == UMI_BUILD_PHASE_RUN && complete.status == UMI_STATUS_OK);
                        CHECK(intent.submitted_operation_id == 2U && atomic_load(&probe.calls) == 2);
                        UmiBuildJobHistoryState next;
                        CHECK(UmiBuildProjectSessionReadHistory(session, &next) == UMI_STATUS_OK &&
                            next.entry.state == UMI_JOB_HISTORY_SUCCEEDED && next.entry.total_steps == 1U);
                        if (logged) CheckFileText(fresh, "program completed\n");
                    }
                    CHECK(UmiBuildRunRestartAdvance(session, profile, root, true, 1U, &submitted) == UMI_STATUS_NOT_FOUND && !submitted);
                    if (logged) CheckFileText(log, "first program stopped\n");
                }
            }
        }
    }
    umi_build_project_session_destroy(session);
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(database);
    free(profile);
    return 0;
}
