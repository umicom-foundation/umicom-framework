/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_reuse/test_session.c
 *
 * PURPOSE:
 *   Verify automatic worker ordering, copied inputs and build failure gating.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/project_session.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
typedef struct Probe { atomic_int calls; bool fail_build; } Probe;
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase,
    UmiCancellationToken *token, UmiBuildResult *result, void *context)
{
    Probe *probe = context; (void)token;
    atomic_fetch_add(&probe->calls, 1);
    if (strcmp(profile->profile_id, "copied") != 0) return UMI_STATUS_INVALID_STATE;
    if (phase == UMI_BUILD_PHASE_BUILD && profile->build_target[0] != '\0')
        return UMI_STATUS_INVALID_STATE;
    result->exit_code = probe->fail_build && phase == UMI_BUILD_PHASE_BUILD ? 1 : 0;
    return result->exit_code == 0 ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
}
static int Wait(UmiBuildProjectSession *session, UmiBuildProjectSessionSnapshot *snapshot)
{
    for (unsigned i = 0U; i < 5000U; ++i) {
        if (umi_build_project_session_snapshot(session, snapshot) != UMI_STATUS_OK) return 0;
        if (!snapshot->active) return 1;
        umi_thread_sleep_ms(1U);
    }
    return 0;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    Probe probe = {0};
    UmiBuildProfile *profile = malloc(sizeof(*profile)); CHECK(profile != NULL);
    CHECK(umi_build_profile_set(profile, "copied", argv[1], "new-build") == UMI_STATUS_OK);
    strcpy(profile->build_target, "one-app");
    UmiBuildProjectSessionConfig config = {NULL, Execute, &probe};
    UmiBuildProjectSession *session = NULL;
    CHECK(umi_build_project_session_create(&config, &session) == UMI_STATUS_OK);
    CHECK(UmiBuildProjectSessionSubmitAutomatic(session, profile, UMI_BUILD_PHASE_BUILD, false, NULL) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(atomic_load(&probe.calls) == 0);
    CHECK(UmiBuildProjectSessionSubmitAutomatic(session, profile, UMI_BUILD_PHASE_BUILD, true, NULL) == UMI_STATUS_OK);
    strcpy(profile->profile_id, "changed-by-caller");
    UmiBuildProjectSessionSnapshot snapshot;
    CHECK(Wait(session, &snapshot));
    CHECK(snapshot.status == UMI_STATUS_OK && snapshot.completed_phase_count == 3U);
    CHECK(snapshot.requested_phase == UMI_BUILD_PHASE_BUILD && snapshot.current_phase == UMI_BUILD_PHASE_TEST);
    CHECK(atomic_load(&probe.calls) == 3);
    strcpy(profile->profile_id, "copied"); probe.fail_build = true;
    /* Snapshot publication can precede task-queue bookkeeping by one turn. */
    UmiStatus submitted = UMI_STATUS_BUSY;
    for (unsigned attempt = 0U; attempt < 5000U && submitted == UMI_STATUS_BUSY; ++attempt) {
        submitted = UmiBuildProjectSessionSubmitAutomatic(session, profile, UMI_BUILD_PHASE_TEST, true, NULL);
        if (submitted == UMI_STATUS_BUSY) umi_thread_sleep_ms(1U);
    }
    CHECK(submitted == UMI_STATUS_OK);
    CHECK(Wait(session, &snapshot));
    CHECK(snapshot.status == UMI_STATUS_IO_ERROR && snapshot.completed_phase_count == 2U);
    CHECK(atomic_load(&probe.calls) == 5);
    /* Once a configured driver exists, it owns all prerequisite stages.
     * The injected executor observes one dispatch, without launching CMake. */
    CHECK(strlen(argv[1]) < sizeof(profile->build_directory));
    strcpy(profile->build_directory, argv[1]);
    probe.fail_build = false;
    submitted = UMI_STATUS_BUSY;
    for (unsigned attempt = 0U; attempt < 5000U && submitted == UMI_STATUS_BUSY; ++attempt) {
        submitted = UmiBuildProjectSessionSubmitAutomatic(session, profile, UMI_BUILD_PHASE_BUILD, true, NULL);
        if (submitted == UMI_STATUS_BUSY) umi_thread_sleep_ms(1U);
    }
    CHECK(submitted == UMI_STATUS_OK && Wait(session, &snapshot));
    CHECK(snapshot.status == UMI_STATUS_OK && snapshot.completed_phase_count == 1U);
    CHECK(snapshot.current_phase == UMI_BUILD_PHASE_TEST && atomic_load(&probe.calls) == 6);
    CHECK(UmiBuildProjectSessionSubmitAutomatic(session, profile, UMI_BUILD_PHASE_INSTALL, true, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    profile->build_testing = 0;
    CHECK(UmiBuildProjectSessionSubmitAutomatic(session, profile, UMI_BUILD_PHASE_TEST, true, NULL) == UMI_STATUS_INVALID_STATE);
    umi_build_project_session_destroy(session); free(profile);
    return 0;
}
