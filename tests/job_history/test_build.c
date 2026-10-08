/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_build.c
 * PURPOSE: Check persistent phase evidence and fail-closed build progression with a controlled executor.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/job_history.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
typedef struct Probe
{
    atomic_int calls;
    atomic_int release;
    const char *name;
    UmiDataServer *server;
} Probe;
/* The executor is a deterministic substitute for a compiler. It exercises the
 * real worker and history ordering without starting any external program. */
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase, UmiCancellationToken *token,
                         UmiBuildResult *result, void *context)
{
    (void)profile;
    (void)phase;
    (void)result;
    Probe *probe = context;
    atomic_fetch_add(&probe->calls, 1);
    if (strcmp(probe->name, "storage-failure") == 0)
    {
        CHECK(umi_data_server_set(probe->server, "job-history/build/next", "corrupt") == UMI_STATUS_OK);
        return UMI_STATUS_OK;
    }
    while (!atomic_load(&probe->release))
    {
        if (umi_cancellation_token_is_requested(token))
            return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    return strcmp(probe->name, "failure") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static void Wait(UmiBuildProjectSession *session)
{
    for (unsigned i = 0; i < 5000U; ++i)
    {
        UmiBuildProjectSessionSnapshot state;
        CHECK(umi_build_project_session_snapshot(session, &state) == UMI_STATUS_OK);
        if (!state.active)
            return;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    Probe probe = {0};
    probe.name = argv[1];
    UmiJobHistory *history = NULL;
    CHECK(umi_data_server_create_memory(&probe.server) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCreate(probe.server, "build", &history) == UMI_STATUS_OK);
    UmiBuildProjectSessionConfig config = {NULL, Execute, &probe};
    UmiBuildProjectSession *session = NULL;
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    UmiBuildJobHistoryState durable;
    CHECK(umi_build_project_session_create(&config, &session) == UMI_STATUS_OK);
    CHECK(UmiBuildProjectSessionSetHistory(session, history) == UMI_STATUS_OK);
    CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_BUILD, false) ==
          UMI_STATUS_PERMISSION_DENIED);
    UmiJobHistorySnapshot *snapshot = calloc(1, sizeof(*snapshot));
    CHECK(snapshot != NULL);
    CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK && snapshot->count == 0);
    if (strcmp(argv[1], "full") == 0)
    {
        UmiJobHistoryEntry entry;
        for (size_t i = 0; i < UMI_JOB_HISTORY_CAPACITY; ++i)
            CHECK(UmiJobHistoryBegin(history, "build", "old", 1, &entry) == UMI_STATUS_OK);
        CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_BUILD, true) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(atomic_load(&probe.calls) == 0);
    }
    else
    {
        if (strcmp(argv[1], "detached") == 0)
            CHECK(UmiBuildProjectSessionSetHistory(session, NULL) == UMI_STATUS_OK);
        CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_BUILD, true) ==
              UMI_STATUS_OK);
        for (unsigned i = 0; i < 5000U && atomic_load(&probe.calls) == 0; ++i)
            umi_thread_sleep_ms(1U);
        CHECK(atomic_load(&probe.calls) != 0);
        if (strcmp(argv[1], "storage-failure") != 0)
        {
            CHECK(UmiBuildProjectSessionSetHistory(session, NULL) == UMI_STATUS_BUSY);
            if (strcmp(argv[1], "cancel") == 0 || strcmp(argv[1], "destroy") == 0)
                umi_build_project_session_cancel(session);
            else
                atomic_store(&probe.release, 1);
        }
        if (strcmp(argv[1], "destroy") == 0)
        {
            umi_build_project_session_destroy(session);
            session = NULL;
        }
        else
        {
            Wait(session);
            CHECK(UmiBuildProjectSessionReadHistory(session, &durable) == UMI_STATUS_OK);
            if (strcmp(argv[1], "storage-failure") == 0)
            {
                CHECK(durable.storage_status == UMI_STATUS_PARSE_ERROR);
                CHECK(durable.entry.state == UMI_JOB_HISTORY_RUNNING);
                CHECK(atomic_load(&probe.calls) == 1);
                UmiBuildResult *result = calloc(1, sizeof(*result));
                CHECK(result != NULL);
                CHECK(umi_build_project_session_result_at(session, 0, result) == UMI_STATUS_OK &&
                      result->status == UMI_STATUS_OK);
                free(result);
                CHECK(umi_data_server_set(probe.server, "job-history/build/next", "1|2") == UMI_STATUS_OK);
            }
            else
                CHECK(durable.storage_status == UMI_STATUS_OK);
        }
        CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK);
        if (strcmp(argv[1], "detached") == 0)
            CHECK(snapshot->count == 0);
        else
        {
            CHECK(snapshot->count == 1);
            UmiJobHistoryState expected = strcmp(argv[1], "storage-failure") == 0 ? UMI_JOB_HISTORY_RUNNING
                                          : strcmp(argv[1], "cancel") == 0 || strcmp(argv[1], "destroy") == 0
                                              ? UMI_JOB_HISTORY_CANCELLED
                                          : strcmp(argv[1], "failure") == 0 ? UMI_JOB_HISTORY_FAILED
                                                                            : UMI_JOB_HISTORY_SUCCEEDED;
            CHECK(snapshot->entries[0].state == expected);
            CHECK(snapshot->entries[0].completed_steps == (expected == UMI_JOB_HISTORY_SUCCEEDED ? 2U
                                                           : expected == UMI_JOB_HISTORY_RUNNING ? 0U
                                                                                                 : 1U));
        }
    }
    umi_build_project_session_destroy(session);
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(probe.server);
    free(snapshot);
    return 0;
}
