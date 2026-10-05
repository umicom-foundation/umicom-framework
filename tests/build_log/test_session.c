/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/build_log/test_session.c
 * PURPOSE: Verify whole-workflow raw capture beyond the live tail and refusal before execution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "fixture.h"
#include "umicom/build/log_capture.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
typedef struct Probe { atomic_int entered; atomic_int release; const char *name; } Probe;
/* Produce bytes without any compiler or shell; pause only after the observer
 * returns so the owner can inspect live file metadata deterministically. */
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase,
    UmiCancellationToken *token, UmiProcessOutputObserver observer, void *observer_context,
    UmiBuildResult *result, void *context)
{
    (void)profile; (void)result; Probe *probe = context;
    unsigned char *bytes = malloc(131072U); CHECK(bytes != NULL);
    memset(bytes, (int)'a' + (int)phase, 131072U); bytes[8] = 0U; bytes[9] = 0xffU;
    observer((const char *)bytes, 131072U, observer_context); free(bytes);
    atomic_store(&probe->entered, 1);
    while (!atomic_load(&probe->release)) {
        if (umi_cancellation_token_is_requested(token)) return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    return strcmp(probe->name, "failure") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}
static UmiStatus Legacy(const UmiBuildProfile *profile, UmiBuildPhase phase,
    UmiCancellationToken *token, UmiBuildResult *result, void *context)
{
    (void)profile; (void)phase; (void)token; (void)context; strcpy(result->output, "final only"); return UMI_STATUS_OK;
}
static void Wait(UmiBuildProjectSession *session)
{
    UmiBuildProjectSessionSnapshot state;
    for (unsigned i = 0U; i < 5000U; ++i) {
        CHECK(umi_build_project_session_snapshot(session, &state) == UMI_STATUS_OK);
        if (!state.active) return;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0);
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1]; char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], next[UMI_PATH_CAPACITY];
    FixtureDirectory(root); FixturePath(path, root, "workflow.log"); FixturePath(next, root, "next.log");
    Probe probe = {0}; probe.name = name;
    UmiBuildProjectSessionConfig config = {NULL, strcmp(name, "legacy") == 0 ? Legacy : NULL, &probe};
    UmiBuildProjectSession *session = NULL; UmiBuildLogSnapshot log;
    UmiBuildProfile profile; umi_build_profile_init(&profile);
    CHECK(UmiBuildProjectSessionCreateObserved(&config, config.execute == NULL ? Execute : NULL, &session) == UMI_STATUS_OK);
    CHECK(UmiBuildProjectSessionReadLog(session, &log) == UMI_STATUS_OK && !log.enabled);
    CHECK(UmiBuildProjectSessionReadLog(NULL, &log) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBuildProjectSessionReadLog(session, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiBuildProjectSessionSubmitLogged(session, &profile, UMI_BUILD_PHASE_BUILD, false, path) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiBuildProjectSessionSubmitLogged(session, &profile, UMI_BUILD_PHASE_BUILD, true, "relative.log") == UMI_STATUS_INVALID_ARGUMENT);
    if (strcmp(name, "collision") == 0) {
        UmiOutputFile *existing = NULL; CHECK(UmiOutputFileCreate(path, &existing) == UMI_STATUS_OK);
        CHECK(UmiOutputFileWrite(existing, "keep", 4U) == UMI_STATUS_OK); UmiOutputFileDestroy(existing);
        CHECK(UmiBuildProjectSessionSubmitLogged(session, &profile, UMI_BUILD_PHASE_BUILD, true, path) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiBuildProjectSessionReadLog(session, &log) == UMI_STATUS_OK && !log.enabled && log.operation_id == 0U);
        CHECK(!atomic_load(&probe.entered));
    } else {
        CHECK(UmiBuildProjectSessionSubmitLogged(session, &profile, UMI_BUILD_PHASE_BUILD, true, path) == UMI_STATUS_OK);
        if (config.execute == NULL) {
            for (unsigned i = 0U; i < 5000U && !atomic_load(&probe.entered); ++i) umi_thread_sleep_ms(1U);
            CHECK(atomic_load(&probe.entered));
            CHECK(UmiBuildProjectSessionReadLog(session, &log) == UMI_STATUS_OK && log.enabled && !log.file.closed);
            CHECK(log.bytes_received == 131072U && log.file.bytes_written == 131072U && !log.capture_complete);
            UmiBuildOutputSnapshot *tail = calloc(1U, sizeof(*tail)); CHECK(tail != NULL);
            CHECK(UmiBuildProjectSessionReadOutput(session, tail) == UMI_STATUS_OK && tail->truncated); free(tail);
            CHECK(UmiBuildProjectSessionSubmitLogged(session, &profile, UMI_BUILD_PHASE_BUILD, true, next) == UMI_STATUS_BUSY);
            UmiOutputFile *unused = NULL; CHECK(UmiOutputFileCreate(next, &unused) == UMI_STATUS_OK); UmiOutputFileDestroy(unused);
            if (strcmp(name, "cancel") == 0 || strcmp(name, "destroy") == 0) umi_build_project_session_cancel(session);
            else atomic_store(&probe.release, 1);
        }
        if (strcmp(name, "destroy") == 0) { umi_build_project_session_destroy(session); session = NULL; }
        else {
            Wait(session);
            CHECK(UmiBuildProjectSessionReadLog(session, &log) == UMI_STATUS_OK && log.file.closed && log.file.status == UMI_STATUS_OK);
            CHECK(log.capture_complete == (config.execute == NULL));
            UmiBuildProjectSessionSnapshot state; CHECK(umi_build_project_session_snapshot(session, &state) == UMI_STATUS_OK);
            if (strcmp(name, "failure") == 0) CHECK(state.status == UMI_STATUS_IO_ERROR);
            else if (strcmp(name, "cancel") == 0) CHECK(state.status == UMI_STATUS_CANCELLED);
            else CHECK(state.status == UMI_STATUS_OK);
            if (strcmp(name, "unlogged-next") == 0) {
                CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_CONFIGURE, true) == UMI_STATUS_OK);
                Wait(session); CHECK(UmiBuildProjectSessionReadLog(session, &log) == UMI_STATUS_OK && !log.enabled);
            }
        }
    }
    umi_build_project_session_destroy(session);
    size_t length; unsigned char *bytes = FixtureRead(path, &length);
    if (strcmp(name, "collision") == 0) CHECK(length == 4U && memcmp(bytes, "keep", 4U) == 0);
    else if (strcmp(name, "legacy") == 0) CHECK(length == 20U && memcmp(bytes, "final onlyfinal only", 20U) == 0);
    else {
        size_t expected = strcmp(name, "cancel") == 0 || strcmp(name, "failure") == 0 || strcmp(name, "destroy") == 0 ? 131072U : 262144U;
        CHECK(length == expected && bytes[8] == 0U && bytes[9] == 0xffU && bytes[0] == 'a');
        if (expected == 262144U) CHECK(bytes[131072U] == 'b' && bytes[131080U] == 0U);
    }
    free(bytes); return 0;
}
