/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/compiler_stream/test_progress.c
 * PURPOSE: Exercise live compiler counts before completion, across phases and on cancellation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/live_diagnostics.h"
#include "umicom/build/live_output.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            exit(1);                                                                               \
        }                                                                                          \
    } while (0)
typedef struct Probe
{
    atomic_int entered;
    atomic_int permit;
    const char *mode;
} Probe;
static void Emit(UmiProcessOutputObserver observer, void *context, const char *text)
{
    observer(text, strlen(text), context);
}
/* The transport is deliberately inert. A latch lets the owner read a running
 * phase before any completed result exists; time alone is not the assertion. */
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase,
                         UmiCancellationToken *token, UmiProcessOutputObserver observer,
                         void *owner, UmiBuildResult *result, void *context)
{
    (void)profile;
    (void)result;
    Probe *probe = context;
    if (phase == UMI_BUILD_PHASE_BUILD)
    {
        Emit(observer, owner, "compile.c:4: note: second phase\n");
    }
    else if (strcmp(probe->mode, "many") == 0)
    {
        for (size_t i = 0U; i < UMI_BUILD_MAX_DIAGNOSTICS + 17U; ++i)
            Emit(observer, owner, "many.c:4: warning: count every complete record\n");
    }
    else if (strcmp(probe->mode, "damaged") == 0)
    {
        const char bytes[] = "bad.c:4: error: prefix\0untrusted suffix\n";
        observer(bytes, sizeof bytes - 1U, owner);
        Emit(observer, owner, "good.c:8: warning: still parse following lines\n");
    }
    else
    {
        Emit(observer, owner, "first.c:2: error: immediate failure\n");
        Emit(observer, owner, "CMake Warning at CMakeLists.txt:3 (message):\n  continuation");
    }
    atomic_store(&probe->entered, (int)phase + 1);
    while (atomic_load(&probe->permit) < (int)phase + 1)
    {
        if (umi_cancellation_token_is_requested(token))
            return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    return strcmp(probe->mode, "failure") == 0 ? UMI_STATUS_INTERNAL_ERROR : UMI_STATUS_OK;
}
static UmiStatus Legacy(const UmiBuildProfile *profile, UmiBuildPhase phase,
                        UmiCancellationToken *token, UmiBuildResult *result, void *context)
{
    (void)profile;
    (void)phase;
    (void)token;
    (void)context;
    strcpy(result->output, "legacy.c:7: warning: bounded final output\n");
    return UMI_STATUS_OK;
}
static void WaitEntered(Probe *probe, UmiBuildPhase phase)
{
    for (unsigned i = 0U; i < 5000U && atomic_load(&probe->entered) != (int)phase + 1; ++i)
        umi_thread_sleep_ms(1U);
    CHECK(atomic_load(&probe->entered) == (int)phase + 1);
}
static void WaitFinished(UmiBuildProjectSession *session)
{
    for (unsigned i = 0U; i < 5000U; ++i)
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
    CHECK(argc == 2);
    Probe probe = {0};
    probe.mode = argv[1];
    UmiBuildProjectSessionConfig config = {NULL, NULL, &probe};
    UmiBuildProjectSession *session = NULL;
    UmiBuildDiagnosticProgress progress, earlier;
    if (strcmp(probe.mode, "legacy") == 0)
        config.execute = Legacy;
    CHECK(UmiBuildProjectSessionCreateObserved(&config, config.execute == NULL ? Execute : NULL,
                                               &session) == UMI_STATUS_OK);
    CHECK(UmiBuildProjectSessionReadDiagnostics(session, &progress) == UMI_STATUS_OK);
    CHECK(progress.operation_id == 0U && progress.errors == 0U);
    if (strcmp(probe.mode, "invalid") == 0)
    {
        CHECK(UmiBuildProjectSessionReadDiagnostics(NULL, &progress) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildProjectSessionReadDiagnostics(session, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else
    {
        UmiBuildProfile profile;
        umi_build_profile_init(&profile);
        UmiBuildPhase requested =
            strcmp(probe.mode, "phases") == 0 ? UMI_BUILD_PHASE_BUILD : UMI_BUILD_PHASE_CONFIGURE;
        CHECK(umi_build_project_session_submit(session, &profile, requested, true) ==
              UMI_STATUS_OK);
        if (config.execute == NULL)
        {
            WaitEntered(&probe, UMI_BUILD_PHASE_CONFIGURE);
            CHECK(UmiBuildProjectSessionReadDiagnostics(session, &progress) == UMI_STATUS_OK);
            CHECK(progress.streamed && !progress.phase_complete && progress.operation_id == 1U);
            CHECK(progress.phase == UMI_BUILD_PHASE_CONFIGURE && progress.phase_index == 0U);
            earlier = progress;
            if (strcmp(probe.mode, "many") == 0)
                CHECK(progress.warnings == UMI_BUILD_MAX_DIAGNOSTICS + 17U);
            else if (strcmp(probe.mode, "damaged") == 0)
                CHECK(progress.unrepresented == 1U && progress.errors == 0U &&
                      progress.warnings == 1U);
            else
                CHECK(progress.errors == 1U && progress.warnings == 0U);
            if (strcmp(probe.mode, "cancel") == 0)
                umi_build_project_session_cancel(session);
            else
                atomic_store(&probe.permit, (int)UMI_BUILD_PHASE_CONFIGURE + 1);
            if (requested == UMI_BUILD_PHASE_BUILD)
            {
                WaitEntered(&probe, UMI_BUILD_PHASE_BUILD);
                CHECK(UmiBuildProjectSessionReadDiagnostics(session, &progress) == UMI_STATUS_OK);
                CHECK(progress.phase_index == 1U && progress.errors == 0U &&
                      progress.warnings == 0U && progress.notes == 1U);
                atomic_store(&probe.permit, (int)UMI_BUILD_PHASE_BUILD + 1);
            }
        }
        WaitFinished(session);
        CHECK(UmiBuildProjectSessionReadDiagnostics(session, &progress) == UMI_STATUS_OK &&
              progress.phase_complete);
        UmiBuildProjectSessionSnapshot state;
        CHECK(umi_build_project_session_snapshot(session, &state) == UMI_STATUS_OK);
        if (strcmp(probe.mode, "legacy") == 0)
            CHECK(!progress.streamed && progress.warnings == 1U);
        else
        {
            CHECK(!earlier.phase_complete); /* Caller snapshots never change retroactively. */
            if (strcmp(probe.mode, "cancel") == 0)
                CHECK(state.status == UMI_STATUS_CANCELLED && progress.warnings == 1U);
            if (strcmp(probe.mode, "failure") == 0)
                CHECK(state.status == UMI_STATUS_INTERNAL_ERROR && progress.warnings == 1U);
            if (strcmp(probe.mode, "early") == 0)
                CHECK(progress.errors == 1U && progress.warnings == 1U &&
                      state.status == UMI_STATUS_OK);
        }
    }

    if (strcmp(probe.mode, "next-run") == 0)
    {
        UmiBuildProfile profile;
        umi_build_profile_init(&profile);
        atomic_store(&probe.entered, 0);
        atomic_store(&probe.permit, 0);
        CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_CONFIGURE,
                                               true) == UMI_STATUS_OK);
        WaitEntered(&probe, UMI_BUILD_PHASE_CONFIGURE);
        CHECK(UmiBuildProjectSessionReadDiagnostics(session, &progress) == UMI_STATUS_OK);
        CHECK(progress.operation_id == 2U && progress.errors == 1U && progress.warnings == 0U &&
              !progress.phase_complete);
        atomic_store(&probe.permit, 1);
        WaitFinished(session);
        CHECK(UmiBuildProjectSessionReadDiagnostics(session, &progress) == UMI_STATUS_OK);
        CHECK(progress.phase_complete && progress.errors == 1U && progress.warnings == 1U);
    }
    umi_build_project_session_destroy(session);
    return 0;
}
