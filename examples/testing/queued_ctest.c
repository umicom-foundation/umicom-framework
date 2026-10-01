/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/testing/queued_ctest.c
 * PURPOSE: Run one chosen CTest test on the shared Framework task queue.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/testing/ctest_job.h"
#include "umicom/platform/threading.h"
#include "umicom/platform/clock.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 3 || argc > 4) {
        (void)fprintf(stderr, "Usage: umicom-ctest-job-example BUILD_DIRECTORY TEST_NAME [STOP_AFTER_MS]\n");
        return 1;
    }
    uint32_t stopAfter = 0U;
    if (argc == 4) {
        char *end = NULL; errno = 0;
        unsigned long value = strtoul(argv[3], &end, 10);
        if (errno != 0 || (argv[3][0] == '\0' || argv[3][0] == '-') || *end != '\0' || value > UINT32_MAX)
            return 1;
        stopAfter = (uint32_t)value;
    }
    UmiCtestJobRequest request = {0};
    if (strlen(argv[1]) >= sizeof(request.build_directory) || strlen(argv[2]) >= sizeof(request.name))
        return 1;
    (void)snprintf(request.build_directory, sizeof(request.build_directory), "%s", argv[1]);
    (void)snprintf(request.name, sizeof(request.name), "%s", argv[2]);
    (void)snprintf(request.test_id, sizeof(request.test_id), "notes.selected");
    (void)snprintf(request.configuration, sizeof(request.configuration), "Debug");
    request.enabled = 1;
    UmiCtestJob *job = NULL;
    UmiTaskQueue *queue = NULL;
    UmiTaskQueueConfig config = {0}; config.worker_count = 1U; config.capacity = 4U;
    UmiStatus status = UmiCtestJobCreate(&request, 1U, NULL, &job);
    if (status == UMI_STATUS_OK) status = umi_task_queue_create(&config, &queue);
    if (status == UMI_STATUS_OK) status = UmiCtestJobSubmit(job, queue);
    UmiClock clock = umi_clock_system();
    uint64_t started = clock.monotonic_nanoseconds(&clock);
    int stopRequested = 0;
    UmiCtestJobSnapshot snapshot = {0};
    while (status == UMI_STATUS_OK) {
        status = UmiCtestJobGetSnapshot(job, &snapshot);
        if (status != UMI_STATUS_OK || snapshot.state >= UMI_TASK_SUCCEEDED) break;
        if (stopAfter != 0U && !stopRequested &&
            (clock.monotonic_nanoseconds(&clock) - started) / UINT64_C(1000000) >= stopAfter) {
            status = UmiCtestJobCancel(job); stopRequested = 1;
        }
        umi_thread_sleep_ms(10U);
    }
    int exitCode = 1;
    if (status == UMI_STATUS_OK) {
        (void)printf("Completed %zu/%zu: %zu passed, %zu failed, %zu skipped, %zu cancelled.\n",
            snapshot.completed, snapshot.planned, snapshot.passed, snapshot.failed,
            snapshot.skipped, snapshot.cancelled);
        UmiTestResult result;
        if (UmiCtestJobResultAt(job, 0U, &result, NULL) == UMI_STATUS_OK)
            (void)printf("%s\n", result.output);
        exitCode = snapshot.status == UMI_STATUS_OK ? 0 : snapshot.status == UMI_STATUS_CANCELLED ? 2 : 1;
    } else (void)fprintf(stderr, "Test run error: %d\n", (int)status);
    if (job != NULL) {
        (void)UmiCtestJobCancel(job);
        while (UmiCtestJobDestroy(job) == UMI_STATUS_BUSY) umi_thread_sleep_ms(1U);
    }
    umi_task_queue_destroy(queue);
    umi_clock_dispose(&clock);
    return exitCode;
}
