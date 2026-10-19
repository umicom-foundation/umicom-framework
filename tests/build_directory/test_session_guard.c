/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_directory/test_session_guard.c
 * Purpose: Verify all-phase ownership and release on completion, refusal, failure and cancellation.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/build/directory_guard.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
typedef struct Probe { atomic_uint entered; atomic_int release; const char *name; } Probe;
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase,
    UmiCancellationToken *token, UmiBuildResult *result, void *context)
{
    (void)phase; (void)result;
    Probe *probe = context;
    if (strcmp(probe->name, "disabled") != 0) {
        UmiFileLease *competing = NULL;
        CHECK(UmiBuildDirectoryGuardAcquire(profile->build_directory, &competing) == UMI_STATUS_BUSY);
        CHECK(competing == NULL);
    }
    atomic_fetch_add(&probe->entered, 1U);
    while (!atomic_load(&probe->release)) {
        if (umi_cancellation_token_is_requested(token)) return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    return strcmp(probe->name, "failure") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static UmiBuildProjectSessionSnapshot Wait(UmiBuildProjectSession *session)
{
    UmiBuildProjectSessionSnapshot state;
    for (unsigned i = 0U; i < 5000U; ++i) {
        CHECK(umi_build_project_session_snapshot(session, &state) == UMI_STATUS_OK);
        if (!state.active) return state;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0); return state;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1]; char root[UMI_PATH_CAPACITY], missing[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(missing, root, "missing");
    Probe probe = {0}; probe.name = name;
    UmiBuildProjectSessionConfig config = {NULL, Execute, &probe};
    UmiBuildProjectSession *session = NULL; UmiFileLease *external = NULL;
    UmiBuildDirectoryGuardSnapshot guard;
    UmiBuildProfile *profile = malloc(sizeof(*profile)); CHECK(profile != NULL);
    CHECK(umi_build_profile_set(profile, "guard", root, root) == UMI_STATUS_OK);
    if (strcmp(name, "missing") == 0) strcpy(profile->build_directory, missing);
    if (strcmp(name, "preset") == 0) strcpy(profile->configure_preset, "chosen");
    CHECK(umi_build_project_session_create(&config, &session) == UMI_STATUS_OK);
    CHECK(UmiBuildProjectSessionReadDirectoryGuard(session, &guard) == UMI_STATUS_OK && !guard.enabled);
    CHECK(UmiBuildProjectSessionSetDirectoryGuard(NULL, true) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBuildProjectSessionReadDirectoryGuard(NULL, &guard) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBuildProjectSessionReadDirectoryGuard(session, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    if (strcmp(name, "disabled") != 0)
        CHECK(UmiBuildProjectSessionSetDirectoryGuard(session, true) == UMI_STATUS_OK);
    if (strcmp(name, "busy") == 0 || strcmp(name, "disabled") == 0)
        CHECK(UmiBuildDirectoryGuardAcquire(root, &external) == UMI_STATUS_OK);
    bool refused = strcmp(name, "busy") == 0 || strcmp(name, "missing") == 0 || strcmp(name, "preset") == 0;
    if (refused || strcmp(name, "disabled") == 0) atomic_store(&probe.release, 1);
    CHECK(umi_build_project_session_submit(session, profile, UMI_BUILD_PHASE_TEST, true) == UMI_STATUS_OK);
    if (!refused && strcmp(name, "disabled") != 0) {
        for (unsigned i = 0U; i < 5000U && atomic_load(&probe.entered) == 0U; ++i) umi_thread_sleep_ms(1U);
        CHECK(atomic_load(&probe.entered) == 1U);
        CHECK(UmiBuildProjectSessionReadDirectoryGuard(session, &guard) == UMI_STATUS_OK);
        CHECK(guard.enabled && guard.attempted && guard.held && guard.acquisition_status == UMI_STATUS_OK);
        CHECK(UmiBuildProjectSessionSetDirectoryGuard(session, false) == UMI_STATUS_BUSY);
        if (strcmp(name, "cancel") == 0 || strcmp(name, "destroy") == 0)
            umi_build_project_session_cancel(session);
        else atomic_store(&probe.release, 1);
    }
    if (strcmp(name, "destroy") == 0) {
        umi_build_project_session_destroy(session); session = NULL;
    } else {
        UmiBuildProjectSessionSnapshot state = Wait(session);
        CHECK(UmiBuildProjectSessionReadDirectoryGuard(session, &guard) == UMI_STATUS_OK);
        CHECK(!guard.held && guard.operation_id == state.operation_id);
        if (refused) {
            CHECK(atomic_load(&probe.entered) == 0U && state.completed_phase_count == 0U);
            UmiStatus expected = strcmp(name, "busy") == 0 ? UMI_STATUS_BUSY :
                strcmp(name, "missing") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_UNAVAILABLE;
            CHECK(state.status == expected && guard.acquisition_status == expected);
            CHECK(guard.attempted);
        } else if (strcmp(name, "cancel") == 0) CHECK(state.status == UMI_STATUS_CANCELLED);
        else if (strcmp(name, "failure") == 0) CHECK(state.status == UMI_STATUS_IO_ERROR);
        else {
            CHECK(state.status == UMI_STATUS_OK && atomic_load(&probe.entered) == 3U);
            CHECK(state.completed_phase_count == 3U);
        }
        if (strcmp(name, "disabled") == 0) CHECK(!guard.enabled && !guard.attempted);
        else CHECK(guard.release_status == UMI_STATUS_OK);
    }
    umi_build_project_session_destroy(session);
    CHECK(UmiFileLeaseRelease(&external) == UMI_STATUS_OK);
    CHECK(UmiBuildDirectoryGuardAcquire(root, &external) == UMI_STATUS_OK);
    CHECK(UmiFileLeaseRelease(&external) == UMI_STATUS_OK);
    free(profile);
    return 0;
}
