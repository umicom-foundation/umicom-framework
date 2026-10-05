/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/live_build_output/test_session.c
 * PURPOSE: Check early publication, bounded bytes and completed-result isolation with an inert executor.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/build/live_output.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); exit(1); } } while (0)
typedef struct Probe { atomic_int entered; atomic_int permit; const char *name; } Probe;

/* This executor cannot start a compiler. The owner releases each phase only
 * after reading the output emitted while that phase is still running. */
static UmiStatus Execute(const UmiBuildProfile *profile, UmiBuildPhase phase,
    UmiCancellationToken *token, UmiProcessOutputObserver observer, void *observer_context,
    UmiBuildResult *result, void *context)
{
    Probe *probe = context;
    if (strcmp(probe->name, "oversized") == 0 || strcmp(probe->name, "rolling") == 0) {
        const size_t capacity = UMI_BUILD_LIVE_OUTPUT_CAPACITY;
        char *chunk = malloc(capacity * 2U); CHECK(chunk != NULL);
        memset(chunk, 'a', capacity * 2U); chunk[capacity * 2U - 1U] = 'z';
        if (strcmp(probe->name, "oversized") == 0) observer(chunk, capacity * 2U, observer_context);
        else { observer(chunk, capacity - 10U, observer_context); observer("0123456789abcdefghij", 20U, observer_context); }
        free(chunk);
    } else if (strcmp(probe->name, "exact") == 0) {
        char *chunk = malloc(UMI_BUILD_LIVE_OUTPUT_CAPACITY - 1U); CHECK(chunk != NULL);
        memset(chunk, 'b', UMI_BUILD_LIVE_OUTPUT_CAPACITY - 1U);
        observer(chunk, UMI_BUILD_LIVE_OUTPUT_CAPACITY - 1U, observer_context); free(chunk);
    } else if (strcmp(probe->name, "empty") == 0) {
        observer("", 0U, observer_context);
    } else if (strcmp(probe->name, "binary") == 0) {
        const char first[] = {'x', '\0', '\xc3'}; const char second[] = {'\xa9', '\xff'};
        observer(first, sizeof(first), observer_context); observer(second, sizeof(second), observer_context);
    } else {
        const char *name = umi_build_phase_text(phase);
        observer(name, strlen(name), observer_context);
        observer("", 0U, observer_context);
    }
    atomic_store(&probe->entered, (int)phase + 1);
    while (atomic_load(&probe->permit) < (int)phase + 1) {
        if (umi_cancellation_token_is_requested(token)) return UMI_STATUS_CANCELLED;
        umi_thread_sleep_ms(1U);
    }
    if (strcmp(probe->name, "phase-boundary-cancel") == 0)
        umi_cancellation_token_request(token);
    (void)snprintf(result->output, sizeof(result->output), "final:%s:%s", profile->profile_id, umi_build_phase_text(phase));
    return strcmp(probe->name, "failure") == 0 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
}

static UmiStatus Legacy(const UmiBuildProfile *profile, UmiBuildPhase phase,
    UmiCancellationToken *token, UmiBuildResult *result, void *context)
{
    (void)profile; (void)phase; (void)token; (void)context;
    strcpy(result->output, "legacy completion"); return UMI_STATUS_OK;
}

static void WaitEntered(Probe *probe, int value)
{
    for (unsigned i = 0U; i < 5000U && atomic_load(&probe->entered) != value; ++i) umi_thread_sleep_ms(1U);
    CHECK(atomic_load(&probe->entered) == value);
}

static void WaitFinished(UmiBuildProjectSession *session)
{
    UmiBuildProjectSessionSnapshot snapshot;
    for (unsigned i = 0U; i < 5000U; ++i) {
        CHECK(umi_build_project_session_snapshot(session, &snapshot) == UMI_STATUS_OK);
        if (!snapshot.active) return;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0);
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1]; Probe probe = {0}; probe.name = name;
    UmiBuildProjectSessionConfig config = {NULL, NULL, &probe};
    UmiBuildProjectSession *session = NULL;
    UmiBuildOutputSnapshot *output = calloc(1U, sizeof(*output));
    UmiBuildOutputSnapshot *captured = calloc(1U, sizeof(*captured));
    UmiBuildResult *result = NULL;
    CHECK(output != NULL && captured != NULL && umi_build_result_create(&result) == UMI_STATUS_OK);
    if (strcmp(name, "legacy") == 0) config.execute = Legacy;
    CHECK(UmiBuildProjectSessionCreateObserved(&config, config.execute == NULL ? Execute : NULL, &session) == UMI_STATUS_OK);
    CHECK(UmiBuildProjectSessionReadOutput(session, output) == UMI_STATUS_OK);
    CHECK(output->operation_id == 0U && output->length == 0U);
    if (strcmp(name, "invalid") == 0) {
        UmiBuildProjectSession *invalid = NULL;
        CHECK(UmiBuildProjectSessionReadOutput(NULL, output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildProjectSessionReadOutput(session, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        config.execute = Legacy;
        CHECK(UmiBuildProjectSessionCreateObserved(&config, Execute, &invalid) == UMI_STATUS_INVALID_ARGUMENT && invalid == NULL);
        CHECK(UmiBuildProjectSessionCreateObserved(NULL, Execute, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBuildProjectSessionCreateObserved(&config, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name, "idle") != 0) {
        UmiBuildProfile profile; umi_build_profile_init(&profile); strcpy(profile.profile_id, "fixture");
        UmiBuildPhase requested = (strcmp(name, "phases") == 0 || strcmp(name, "phase-boundary-cancel") == 0) ? UMI_BUILD_PHASE_BUILD : UMI_BUILD_PHASE_CONFIGURE;
        CHECK(umi_build_project_session_submit(session, &profile, requested, false) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiBuildProjectSessionReadOutput(session, output) == UMI_STATUS_OK && output->operation_id == 0U);
        CHECK(umi_build_project_session_submit(session, &profile, requested, true) == UMI_STATUS_OK);
        if (config.execute == NULL) {
            WaitEntered(&probe, (int)UMI_BUILD_PHASE_CONFIGURE + 1);
            CHECK(UmiBuildProjectSessionReadOutput(session, output) == UMI_STATUS_OK);
            CHECK(output->operation_id == 1U && output->streamed && !output->phase_complete);
            CHECK(umi_build_project_session_result_at(session, 0U, result) == UMI_STATUS_NOT_FOUND);
            CHECK(umi_build_project_session_submit(session, &profile, requested, true) == UMI_STATUS_BUSY);
            *captured = *output;
            if (strcmp(name, "oversized") == 0) {
                CHECK(output->length == UMI_BUILD_LIVE_OUTPUT_CAPACITY - 1U && output->truncated);
                CHECK(output->total_bytes == UMI_BUILD_LIVE_OUTPUT_CAPACITY * 2U && output->bytes[output->length - 1U] == 'z');
            } else if (strcmp(name, "rolling") == 0) {
                CHECK(output->truncated && output->total_bytes == UMI_BUILD_LIVE_OUTPUT_CAPACITY + 10U);
                CHECK(memcmp(output->bytes + output->length - 20U, "0123456789abcdefghij", 20U) == 0);
            } else if (strcmp(name, "exact") == 0) {
                CHECK(output->length == UMI_BUILD_LIVE_OUTPUT_CAPACITY - 1U && !output->truncated);
                CHECK(output->bytes[output->length] == '\0');
            } else if (strcmp(name, "empty") == 0) {
                CHECK(output->length == 0U && output->total_bytes == 0U && !output->truncated);
            } else if (strcmp(name, "binary") == 0) {
                const char expected[] = {'x', '\0', '\xc3', '\xa9', '\xff'};
                CHECK(output->length == sizeof(expected) && memcmp(output->bytes, expected, sizeof(expected)) == 0);
            }
            if (strcmp(name, "cancel") == 0) umi_build_project_session_cancel(session);
            else atomic_store(&probe.permit, (int)UMI_BUILD_PHASE_CONFIGURE + 1);
            if (strcmp(name, "phases") == 0) {
                WaitEntered(&probe, (int)UMI_BUILD_PHASE_BUILD + 1);
                CHECK(UmiBuildProjectSessionReadOutput(session, output) == UMI_STATUS_OK);
                CHECK(output->phase_index == 1U && !output->phase_complete && output->phase == UMI_BUILD_PHASE_BUILD);
                CHECK(strcmp(output->bytes, umi_build_phase_text(UMI_BUILD_PHASE_BUILD)) == 0);
                CHECK(output->revision > captured->revision);
                CHECK(umi_build_project_session_result_at(session, 0U, result) == UMI_STATUS_OK);
                atomic_store(&probe.permit, (int)UMI_BUILD_PHASE_BUILD + 1);
            }
        }
        WaitFinished(session);
        CHECK(UmiBuildProjectSessionReadOutput(session, output) == UMI_STATUS_OK && output->phase_complete);
        if (strcmp(name, "failure") == 0) CHECK(output->status == UMI_STATUS_IO_ERROR);
        else if (strcmp(name, "cancel") == 0) CHECK(output->status == UMI_STATUS_CANCELLED);
        else CHECK(output->status == UMI_STATUS_OK);
        if (strcmp(name, "phase-boundary-cancel") == 0) {
            UmiBuildProjectSessionSnapshot progress;
            CHECK(umi_build_project_session_snapshot(session, &progress) == UMI_STATUS_OK);
            CHECK(progress.status == UMI_STATUS_CANCELLED && progress.completed_phase_count == 1U);
            CHECK(output->phase == UMI_BUILD_PHASE_CONFIGURE && output->status == UMI_STATUS_OK);
        }
        if (strcmp(name, "legacy") == 0) CHECK(!output->streamed && strcmp(output->bytes, "legacy completion") == 0);
        else CHECK(!captured->phase_complete); /* Completion cannot mutate an earlier caller copy. */
        if (strcmp(name, "repeat") == 0) {
            atomic_store(&probe.entered, 0); atomic_store(&probe.permit, 0);
            CHECK(umi_build_project_session_submit(session, &profile, UMI_BUILD_PHASE_CONFIGURE, true) == UMI_STATUS_OK);
            WaitEntered(&probe, (int)UMI_BUILD_PHASE_CONFIGURE + 1);
            CHECK(UmiBuildProjectSessionReadOutput(session, output) == UMI_STATUS_OK);
            CHECK(output->operation_id == 2U && output->total_bytes == strlen(umi_build_phase_text(UMI_BUILD_PHASE_CONFIGURE)));
            /* Destruction must cancel and join a blocked executor before freeing its buffer. */
        }
    }
    umi_build_project_session_destroy(session); umi_build_result_destroy(result);
    free(output); free(captured); return 0;
}
