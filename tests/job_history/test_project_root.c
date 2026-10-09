/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_project_root.c
 * PURPOSE: Keep queued build phases on their submitted project after the process directory changes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/build/job_history.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/path.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
typedef struct Probe
{
    atomic_int ready, release, calls, wrong;
    char root[UMI_BUILD_PATH_CAPACITY];
} Probe;
static void ChangeDirectory(const char *path)
{
#ifdef _WIN32
    wchar_t wide[UMI_PATH_CAPACITY];
    CHECK(MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, path, -1, wide,
                              (int)UMI_PATH_CAPACITY) != 0);
    CHECK(SetCurrentDirectoryW(wide));
#else
    CHECK(chdir(path) == 0);
#endif
}
/* The controlled executor runs no compiler. It waits until the owner changes
 * cwd, then resolves the supplied profile exactly as a later build phase does. */
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase,
                         UmiCancellationToken *token, UmiBuildResult *result, void *context)
{
    (void)phase;
    (void)result;
    Probe *probe = context;
    atomic_store(&probe->ready, 1);
    while (!atomic_load(&probe->release))
    {
        if (umi_cancellation_token_is_requested(token))
            return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    char actual[UMI_BUILD_PATH_CAPACITY];
    if (!umi_path_is_absolute(profile->source_directory) ||
        UmiBuildProfileSourceDirectory(profile, actual, sizeof(actual)) != UMI_STATUS_OK ||
        !umi_path_equal(actual, probe->root))
        atomic_store(&probe->wrong, 1);
    atomic_fetch_add(&probe->calls, 1);
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    bool withHistory = strcmp(argv[1], "history") == 0;
    CHECK(withHistory || strcmp(argv[1], "plain") == 0);
    Probe probe = {0};
    char other[UMI_PATH_CAPACITY];
    CHECK(umi_fs_current_directory(probe.root, sizeof(probe.root)) == UMI_STATUS_OK);
    FixtureDirectory(other);
    UmiDataServer *server = NULL;
    UmiJobHistory *history = NULL;
    UmiBuildProjectSessionConfig config = {NULL, Execute, &probe};
    UmiBuildProjectSession *session = NULL;
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    UmiJobIdentity submitted;
    CHECK(UmiBuildProfileJobIdentity(&profile, &submitted) == UMI_STATUS_OK);
    CHECK(umi_build_project_session_create(&config, &session) == UMI_STATUS_OK);
    if (withHistory)
    {
        CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
        CHECK(UmiJobHistoryCreate(server, "root", &history) == UMI_STATUS_OK);
        CHECK(UmiBuildProjectSessionSetHistory(session, history) == UMI_STATUS_OK);
    }
    CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_BUILD, true) ==
          UMI_STATUS_OK);
    for (unsigned i = 0U; i < 5000U && !atomic_load(&probe.ready); ++i)
        umi_thread_sleep_ms(1U);
    CHECK(atomic_load(&probe.ready));
    ChangeDirectory(other);
    atomic_store(&probe.release, 1);
    UmiBuildProjectSessionSnapshot snapshot = {0};
    for (unsigned i = 0U; i < 5000U; ++i)
    {
        CHECK(umi_build_project_session_snapshot(session, &snapshot) == UMI_STATUS_OK);
        if (!snapshot.active)
            break;
        umi_thread_sleep_ms(1U);
    }
    ChangeDirectory(probe.root);
    CHECK(!snapshot.active && snapshot.status == UMI_STATUS_OK);
    CHECK(atomic_load(&probe.calls) == 2 && !atomic_load(&probe.wrong));
    CHECK(strcmp(profile.source_directory, ".") == 0);
    if (withHistory)
    {
        UmiBuildJobHistoryState state;
        CHECK(UmiBuildProjectSessionReadHistory(session, &state) == UMI_STATUS_OK);
        CHECK(UmiJobIdentityCompare(&state.entry.identity, &submitted) ==
              UMI_JOB_IDENTITY_INPUTS_UNRECORDED);
    }
    umi_build_project_session_destroy(session);
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    return 0;
}
