/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_build_sqlite.c
 * PURPOSE: Refuse external build phases when real SQLite checkpoint writes fail.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/build/job_history.h"
#include "umicom/platform/threading.h"
#ifdef UMICOM_HAS_SQLITE
typedef struct Probe
{
    UmiDataServer *server;
    unsigned calls;
    unsigned fail_after;
} Probe;
/* Refuse updates but allow the first prepared record to be inserted. This
 * exercises SQLite error handling without pretending a compiler has failed. */
static void RefuseUpdates(UmiDataServer *server)
{
    CHECK(umi_data_server_execute(
              server, "CREATE TEMP TRIGGER refuse_job_checkpoint BEFORE UPDATE ON umicom_kv "
                      "WHEN NEW.key LIKE 'job-history/build/slot/%' "
                      "BEGIN SELECT RAISE(ABORT,'fixture checkpoint failure'); END;") == UMI_STATUS_OK);
}
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase, UmiCancellationToken *token,
                         UmiBuildResult *result, void *context)
{
    (void)profile;
    (void)phase;
    (void)token;
    (void)result;
    Probe *probe = context;
    ++probe->calls;
    if (probe->calls == probe->fail_after)
        RefuseUpdates(probe->server);
    return UMI_STATUS_OK;
}
#endif
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
#ifndef UMICOM_HAS_SQLITE
    (void)argv;
    return 77;
#else
    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(path, directory, "build-jobs.sqlite");
    Probe probe = {0};
    probe.fail_after = strcmp(argv[1], "before-start") == 0     ? 0U
                       : strcmp(argv[1], "between-phases") == 0 ? 1U
                                                                : 2U;
    CHECK(strcmp(argv[1], "before-start") == 0 || strcmp(argv[1], "between-phases") == 0 ||
          strcmp(argv[1], "final-outcome") == 0);
    CHECK(umi_data_server_create_sqlite(path, &probe.server) == UMI_STATUS_OK);
    UmiJobHistory *history = NULL;
    CHECK(UmiJobHistoryCreate(probe.server, "build", &history) == UMI_STATUS_OK);
    if (probe.fail_after == 0U)
        RefuseUpdates(probe.server);
    UmiBuildProjectSessionConfig config = {NULL, Execute, &probe};
    UmiBuildProjectSession *session = NULL;
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    CHECK(umi_build_project_session_create(&config, &session) == UMI_STATUS_OK);
    CHECK(UmiBuildProjectSessionSetHistory(session, history) == UMI_STATUS_OK);
    CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_BUILD, true) == UMI_STATUS_OK);
    UmiBuildProjectSessionSnapshot progress = {0};
    for (unsigned i = 0; i < 5000U; ++i)
    {
        CHECK(umi_build_project_session_snapshot(session, &progress) == UMI_STATUS_OK);
        if (!progress.active)
            break;
        umi_thread_sleep_ms(1U);
    }
    CHECK(!progress.active && progress.status != UMI_STATUS_OK);
    CHECK(progress.completed_phase_count == probe.fail_after);
    UmiBuildJobHistoryState durable;
    CHECK(UmiBuildProjectSessionReadHistory(session, &durable) == UMI_STATUS_OK);
    CHECK(durable.enabled && durable.storage_status != UMI_STATUS_OK);
    UmiBuildResult *result = calloc(1, sizeof(*result));
    CHECK(result != NULL);
    for (unsigned i = 0; i < probe.fail_after; ++i)
        CHECK(umi_build_project_session_result_at(session, i, result) == UMI_STATUS_OK &&
              result->status == UMI_STATUS_OK);
    CHECK(umi_build_project_session_result_at(session, probe.fail_after, result) == UMI_STATUS_NOT_FOUND);
    free(result);
    umi_build_project_session_destroy(session);
    /* Destruction joins the worker before inspecting its non-atomic probe. */
    CHECK(probe.calls == probe.fail_after);
    UmiJobHistorySnapshot *saved = calloc(1, sizeof(*saved));
    CHECK(saved != NULL);
    CHECK(UmiJobHistoryCapture(history, saved) == UMI_STATUS_OK && saved->count == 1 &&
          saved->unfinished_count == 1);
    CHECK(saved->entries[0].state ==
          (probe.fail_after == 0U ? UMI_JOB_HISTORY_PREPARED : UMI_JOB_HISTORY_RUNNING));
    CHECK(saved->entries[0].completed_steps == (probe.fail_after == 2U ? 1U : 0U));
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(probe.server);
    free(saved);
    return 0;
#endif
}
