/*-----------------------------------------------------------------------------
 * Umicom Framework / Studio qualification
 * File: framework/tests/developer_project/test_project_session.c
 * PURPOSE: Verify asynchronous phase ordering, trust, copied input and cancellation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return EXIT_FAILURE; } } while (0)

#include <stdatomic.h>
#include "umicom/build/project_session.h"
#include "umicom/platform/threading.h"
typedef struct Probe {
    atomic_int entered;
    atomic_int release;
    int fail_build;
} Probe;
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase,
    UmiCancellationToken *token, UmiBuildResult *result, void *context)
{
    Probe *probe = context;
    atomic_store(&probe->entered, 1);
    while (!atomic_load(&probe->release)) {
        if (umi_cancellation_token_is_requested(token)) return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    snprintf(result->output, sizeof(result->output), "%s|%s|%s", profile->profile_id,
        umi_build_phase_text(phase), profile->build_target);
    return probe->fail_build && phase == UMI_BUILD_PHASE_BUILD ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static int Wait(UmiBuildProjectSession *session, UmiBuildProjectSessionSnapshot *state)
{
    for (unsigned attempt = 0U; attempt < 5000U; ++attempt) {
        if (umi_build_project_session_snapshot(session, state) != UMI_STATUS_OK) return 0;
        if (!state->active) return 1;
        umi_thread_sleep_ms(1U);
    }
    return 0;
}
int main(int argc, char **argv)
{
    Probe probe = {0};
    UmiBuildProjectSessionConfig config = {NULL, Execute, &probe};
    UmiBuildProjectSession *session = NULL;
    UmiBuildProjectSessionSnapshot state;
    UmiBuildProfile profile;
    UmiBuildResult *result = NULL;
    int cancel = argc == 2 && strcmp(argv[1], "cancel") == 0;
    int failure = argc == 2 && strcmp(argv[1], "failure") == 0;
    umi_build_profile_init(&profile);
    strcpy(profile.profile_id, "umicom-notes");
    strcpy(profile.run_program, "notes");
    strcpy(profile.build_target, "notes");
    CHECK(umi_build_result_create(&result) == UMI_STATUS_OK);
    CHECK(umi_build_project_session_create(&config, &session) == UMI_STATUS_OK);
    CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_RUN, false) == UMI_STATUS_PERMISSION_DENIED);
    probe.fail_build = failure;
    CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_RUN, true) == UMI_STATUS_OK);
    CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_BUILD, true) == UMI_STATUS_BUSY);
    strcpy(profile.profile_id, "caller-changed-after-submit");
    if (cancel) umi_build_project_session_cancel(session);
    else atomic_store(&probe.release, 1);
    CHECK(Wait(session, &state));
    if (cancel) {
        CHECK(state.status == UMI_STATUS_CANCELLED);
        CHECK(state.cancellation_requested);
    } else if (failure) {
        CHECK(state.status == UMI_STATUS_IO_ERROR);
        CHECK(state.completed_phase_count == 2U);
    } else {
        CHECK(state.status == UMI_STATUS_OK && state.completed_phase_count == 3U);
        CHECK(umi_build_project_session_result_at(session, 0U, result) == UMI_STATUS_OK);
        CHECK(result->phase == UMI_BUILD_PHASE_CONFIGURE);
        CHECK(strstr(result->output, "umicom-notes") != NULL);
        CHECK(umi_build_project_session_result_at(session, 1U, result) == UMI_STATUS_OK);
        CHECK(result->phase == UMI_BUILD_PHASE_BUILD);
        CHECK(umi_build_project_session_result_at(session, 2U, result) == UMI_STATUS_OK);
        CHECK(result->phase == UMI_BUILD_PHASE_RUN);
        CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_TEST, true) == UMI_STATUS_OK);
        CHECK(Wait(session, &state));
        CHECK(state.completed_phase_count == 3U);
        CHECK(umi_build_project_session_result_at(session, 1U, result) == UMI_STATUS_OK);
        CHECK(result->output[strlen(result->output)-1U] == '|'); /* Build ALL before testing. */
        CHECK(umi_build_project_session_result_at(session, 2U, result) == UMI_STATUS_OK);
        CHECK(result->phase == UMI_BUILD_PHASE_TEST);
        CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_INSTALL, true) == UMI_STATUS_OK);
        CHECK(Wait(session, &state));
        CHECK(umi_build_project_session_result_at(session, 2U, result) == UMI_STATUS_OK);
        CHECK(result->phase == UMI_BUILD_PHASE_INSTALL);
        CHECK(umi_build_project_session_result_at(session, 3U, result) == UMI_STATUS_NOT_FOUND);
    }
    umi_build_project_session_destroy(session);
    umi_build_result_destroy(result);
    puts("Background project session passed.");
    return EXIT_SUCCESS;
}
