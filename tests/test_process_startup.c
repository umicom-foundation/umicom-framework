/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_process_startup.c
 *
 * PURPOSE:
 *   Exercise native creation, bounded diagnostics and activation refusal using only inert test children.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/platform/process_supervisor.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

static int Child(const char *mode)
{
    if (strcmp(mode, "--silent") == 0) { umi_thread_sleep_ms(900U); return 0; }
    if (strcmp(mode, "--failure") == 0) { fputs("startup fixture rejected configuration\n", stderr); return 7; }
    if (strcmp(mode, "--long") == 0) {
        for (unsigned i = 0U; i < 7000U; ++i) fputc('x', stderr);
        fputs(" LAST-DIAGNOSTIC\n", stderr); return 9;
    }
    fputs("live startup diagnostic\n", stderr); fflush(stderr);
    umi_thread_sleep_ms(900U); return 0;
}

static int FormatCases(void)
{
    UmiProcessJobSnapshot snapshot = {0};
    char text[512], before[512];
    memset(text, 'Q', sizeof(text)); memcpy(before, text, sizeof(text));
    CHECK(UmiProcessJobFormatCompletion(&snapshot, text, sizeof(text)) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(text, before, sizeof(text)) == 0);
    snapshot.state = UMI_PROCESS_JOB_FAILED;
    snapshot.exit_code = (int)UINT32_C(0xc0000135);
    CHECK(UmiProcessJobFormatCompletion(&snapshot, text, sizeof(text)) == UMI_STATUS_OK);
    CHECK(strstr(text, "0xC0000135") != NULL && strstr(text, "missing DLL") != NULL);
    memcpy(before, text, sizeof(text));
    CHECK(UmiProcessJobFormatCompletion(&snapshot, text, 5U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(text, before, sizeof(text)) == 0);
    memset(snapshot.output, 'z', sizeof(snapshot.output));
    CHECK(UmiProcessJobFormatCompletion(&snapshot, text, sizeof(text)) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(text, before, sizeof(text)) == 0);
    snapshot.output[sizeof(snapshot.output) - 1U] = '\0';
    CHECK(UmiProcessJobFormatCompletion(&snapshot, text, sizeof(text)) == UMI_STATUS_OK);
    CHECK(strlen(text) < sizeof(text) && strstr(text, "truncated") != NULL);
    CHECK(strstr(text, "0xC0000135") != NULL);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strncmp(argv[1], "--", 2U) == 0) return Child(argv[1]);
    CHECK(argc == 3);
    if (strcmp(argv[1], "format") == 0) return FormatCases();
    UmiProcessSupervisor *owner = NULL;
    UmiProcessSupervisorConfig config = umi_process_supervisor_config_default();
    config.capacity = 1U;
    CHECK(umi_process_supervisor_create(&config, &owner) == UMI_STATUS_OK);
    int launched = 31;
    CHECK(UmiProcessSupervisorLaunchObserved(NULL, 1U, &launched) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiProcessSupervisorLaunchObserved(owner, 1U, &launched) == UMI_STATUS_NOT_FOUND && launched == 31);
    CHECK(UmiProcessSupervisorActivate(owner, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiProcessSupervisorActivate(owner, 1U) == UMI_STATUS_NOT_FOUND);
    const char *mode = strcmp(argv[1], "failure") == 0 ? "--failure" :
        strcmp(argv[1], "long") == 0 ? "--long" :
        strcmp(argv[1], "live") == 0 ? "--live" : "--silent";
    const char *arguments[] = {mode};
    UmiProcessRequest request = {0};
    char missing[4096];
    (void)snprintf(missing, sizeof(missing), "%s.missing-native-fixture", argv[2]);
    int absent = strcmp(argv[1], "missing") == 0;
    request.program = absent ? missing : argv[2];
    request.arguments = arguments; request.argument_count = 1U;
    request.capture_stdout = 1; request.capture_stderr = 1;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    request.timeout_ms = 5000U; request.poll_interval_ms = 2U;
    UmiProcessJobId job = 0U;
    CHECK(umi_process_supervisor_submit(owner, "inert startup child", &request, &job) == UMI_STATUS_OK);
    UmiProcessJobSnapshot snapshot;
    int observedLive = 0, observedOutput = 0, checkedActivation = 0;
    for (unsigned attempt = 0U; attempt < 5000U; ++attempt) {
        CHECK(umi_process_supervisor_snapshot(owner, job, &snapshot) == UMI_STATUS_OK);
        CHECK(UmiProcessSupervisorLaunchObserved(owner, job, &launched) == UMI_STATUS_OK);
        if (snapshot.state == UMI_PROCESS_JOB_RUNNING && launched) {
            observedLive = 1;
            if (!checkedActivation && strcmp(mode, "--silent") == 0) {
#ifdef _WIN32
                CHECK(UmiProcessSupervisorActivate(owner, job) == UMI_STATUS_UNAVAILABLE);
#else
                CHECK(UmiProcessSupervisorActivate(owner, job) == UMI_STATUS_NOT_IMPLEMENTED);
#endif
                checkedActivation = 1;
            }
            if (strstr(snapshot.output, "live startup") != NULL) observedOutput = 1;
        }
        if (snapshot.state >= UMI_PROCESS_JOB_SUCCEEDED) break;
        umi_thread_sleep_ms(2U);
    }
    CHECK(snapshot.state >= UMI_PROCESS_JOB_SUCCEEDED);
    CHECK(UmiProcessSupervisorActivate(owner, job) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiProcessSupervisorLaunchObserved(owner, job, &launched) == UMI_STATUS_OK);
    CHECK(launched == !absent);
    char message[512];
    CHECK(UmiProcessJobFormatCompletion(&snapshot, message, sizeof(message)) == UMI_STATUS_OK);
    if (absent) {
        CHECK(snapshot.state == UMI_PROCESS_JOB_FAILED);
#ifdef _WIN32
/* Missing files are rejected by the shared path preflight before CreateProcessW. Assert that specific preserved diagnostic, not a loader invocation that never occurred. The former implementation is retained for owner review. */
#if 0
        CHECK(strstr(message, "Windows could not create") != NULL);
#endif
        CHECK(strstr(message, "Windows process preparation failed") != NULL);
#endif
    } else if (strcmp(mode, "--failure") == 0) {
        CHECK(snapshot.exit_code == 7 && strstr(message, "rejected configuration") != NULL);
    } else if (strcmp(mode, "--long") == 0) {
        CHECK(snapshot.exit_code == 9 && strstr(message, "LAST-DIAGNOSTIC") != NULL);
        CHECK(strstr(message, "truncated") != NULL);
    } else {
        CHECK(observedLive && snapshot.exit_code == 0);
        if (strcmp(mode, "--live") == 0) CHECK(observedOutput);
    }
    CHECK(UmiProcessSupervisorReleaseJob(owner, job) == UMI_STATUS_OK);
    CHECK(UmiProcessSupervisorActivate(owner, job) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiProcessSupervisorRelease(&owner) == UMI_STATUS_OK && owner == NULL);
    return 0;
}
