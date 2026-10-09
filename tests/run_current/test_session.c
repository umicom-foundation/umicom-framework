/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/run_current/test_session.c
 * PURPOSE: Verify one-stage launch ownership, cancellation, copied settings and job evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/build/job_history.h"
#include "umicom/build/log_capture.h"
#include <stdatomic.h>
typedef struct LaunchProbe
{
    atomic_int calls, permit;
    bool fail;
} LaunchProbe;
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase,
                         UmiCancellationToken *token, UmiBuildResult *result, void *context)
{
    LaunchProbe *probe = context;
    atomic_fetch_add(&probe->calls, 1);
    CHECK(phase == UMI_BUILD_PHASE_RUN);
    CHECK(umi_path_is_absolute(profile->run_program));
    CHECK(umi_path_is_absolute(profile->run_working_directory));
    CHECK(strcmp(profile->run_environment, "UMICOM_RUN_VALUE=selected") == 0);
    while (!atomic_load(&probe->permit))
    {
        if (umi_cancellation_token_is_requested(token))
            return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    strcpy(result->output, "one selected program\n");
    return probe->fail ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    LaunchProbe probe = {0};
    UmiBuildProfile *profile = malloc(sizeof *profile);
    CHECK(profile != NULL);
    FixtureProfile(profile);
    UmiBuildProjectSessionConfig config = {NULL, Execute, &probe};
    UmiBuildProjectSession *session = NULL;
    UmiDataServer *server = NULL;
    UmiJobHistory *history = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCreate(server, "launch", &history) == UMI_STATUS_OK);
    CHECK(umi_build_project_session_create(&config, &session) == UMI_STATUS_OK);
    CHECK(UmiBuildProjectSessionSetHistory(session, history) == UMI_STATUS_OK);
    CHECK(UmiBuildProjectSessionRunCurrent(session, profile, false, NULL) ==
          UMI_STATUS_PERMISSION_DENIED);
    UmiBuildProjectSessionSnapshot state;
    CHECK(umi_build_project_session_snapshot(session, &state) == UMI_STATUS_OK &&
          state.operation_id == 0U);
    const char *log_path = NULL;
    if (strcmp(argv[1], "log-existing") == 0)
    {
        CHECK(umi_fs_write_text("existing.log", "retain this evidence\n") == UMI_STATUS_OK);
        CHECK(UmiBuildProjectSessionRunCurrent(session, profile, true, "existing.log") !=
              UMI_STATUS_OK);
        CHECK(atomic_load(&probe.calls) == 0);
        char *text = NULL;
        size_t length = 0U;
        CHECK(umi_fs_read_text("existing.log", &text, &length) == UMI_STATUS_OK);
        CHECK(strcmp(text, "retain this evidence\n") == 0);
        umi_fs_free_text(text);
    }
    else
    {
        bool cancel = strcmp(argv[1], "cancel") == 0;
        probe.fail = strcmp(argv[1], "failure") == 0;
        if (strcmp(argv[1], "log") == 0)
            log_path = "new.log";
        CHECK(cancel || probe.fail || log_path != NULL || strcmp(argv[1], "copied") == 0 ||
              strcmp(argv[1], "repeat") == 0);
        CHECK(UmiBuildProjectSessionRunCurrent(session, profile, true, log_path) == UMI_STATUS_OK);
        /* Input edits after queue acceptance cannot retarget the running job. */
        strcpy(profile->run_program, "./missing-program");
        strcpy(profile->run_environment, "UMICOM_RUN_VALUE=changed");
        for (unsigned attempt = 0U; attempt < 5000U && atomic_load(&probe.calls) == 0; ++attempt)
            umi_thread_sleep_ms(1U);
        CHECK(atomic_load(&probe.calls) == 1);
        CHECK(UmiBuildProjectSessionRunCurrent(session, profile, true, NULL) == UMI_STATUS_BUSY);
        if (cancel)
            umi_build_project_session_cancel(session);
        else
            atomic_store(&probe.permit, 1);
        state = FixtureWait(session);
        CHECK(state.status == (cancel       ? UMI_STATUS_CANCELLED
                               : probe.fail ? UMI_STATUS_IO_ERROR
                                            : UMI_STATUS_OK));
        CHECK(state.completed_phase_count == 1U && state.current_phase == UMI_BUILD_PHASE_RUN);
        UmiBuildResult *result = malloc(sizeof *result);
        CHECK(result != NULL);
        CHECK(umi_build_project_session_result_at(session, 0U, result) == UMI_STATUS_OK);
        CHECK(result->phase == UMI_BUILD_PHASE_RUN);
        CHECK(umi_build_project_session_result_at(session, 1U, result) == UMI_STATUS_NOT_FOUND);
        free(result);
        UmiBuildJobHistoryState durable;
        CHECK(UmiBuildProjectSessionReadHistory(session, &durable) == UMI_STATUS_OK);
        CHECK(durable.entry.total_steps == 1U && durable.entry.completed_steps == 1U);
        CHECK(strcmp(durable.entry.kind, "build.launch") == 0);
        CHECK(durable.entry.state == (cancel       ? UMI_JOB_HISTORY_CANCELLED
                                      : probe.fail ? UMI_JOB_HISTORY_FAILED
                                                   : UMI_JOB_HISTORY_SUCCEEDED));
        if (log_path != NULL)
        {
            char *text = NULL;
            size_t length = 0U;
            CHECK(umi_fs_read_text(log_path, &text, &length) == UMI_STATUS_OK);
            CHECK(strstr(text, "one selected program") != NULL);
            umi_fs_free_text(text);
            CHECK(umi_fs_remove_tree(log_path) == UMI_STATUS_OK);
        }
        if (strcmp(argv[1], "repeat") == 0)
        {
            FixtureProfile(profile);
            CHECK(UmiBuildProjectSessionRunCurrent(session, profile, true, NULL) == UMI_STATUS_OK);
            UmiBuildProjectSessionSnapshot next = FixtureWait(session);
            CHECK(next.operation_id == state.operation_id + 1U && next.completed_phase_count == 1U);
            CHECK(atomic_load(&probe.calls) == 2);
        }
    }
    umi_build_project_session_destroy(session);
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    free(profile);
    return 0;
}
