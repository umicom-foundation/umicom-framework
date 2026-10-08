/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/live_test_output/test_execution.c
 * PURPOSE: Exercise real streamed CTest invocations before completion and after cancellation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/ctest_output.h"
#include "umicom/platform/threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                       \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)

static char release_path[2048];
static void release_child(void)
{
    FILE *file = fopen(release_path, "wb");
    CHECK(file != NULL);
    CHECK(fputs("release\n", file) >= 0);
    CHECK(fclose(file) == 0);
}
typedef struct Observation
{
    UmiOutputTailState tail;
    char bytes[65536];
    int released;
} Observation;
static void observe(const char *bytes, size_t length, void *context)
{
    Observation *capture = context;
    CHECK(UmiOutputTailAppend(capture->bytes, sizeof(capture->bytes), &capture->tail, bytes, length) ==
          UMI_STATUS_OK);
    if (!capture->released && strstr(capture->bytes, "LIVE OUTPUT BEGIN") != NULL)
    {
        release_child();
        capture->released = 1;
    }
}
static UmiCtestJobSnapshot progress(UmiCtestJob *job)
{
    UmiCtestJobSnapshot snapshot;
    CHECK(UmiCtestJobGetSnapshot(job, &snapshot) == UMI_STATUS_OK);
    return snapshot;
}
static void wait_for_job(UmiCtestJob *job)
{
    for (unsigned poll = 0U; poll < 15000U; ++poll)
    {
        if (progress(job).state >= UMI_TASK_SUCCEEDED)
            return;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0);
}
static void wait_for_output(UmiCtestJob *job, UmiCtestOutputSnapshot *output)
{
    for (unsigned poll = 0U; poll < 10000U; ++poll)
    {
        CHECK(UmiCtestJobReadOutput(job, output) == UMI_STATUS_OK);
        if (strstr(output->bytes, "LIVE OUTPUT BEGIN") != NULL)
            return;
        umi_thread_sleep_ms(1U);
    }
    CHECK(0);
}
int main(int argc, char **argv)
{
    CHECK(argc == 3);
    const char *mode = argv[1], *root = argv[2];
    CHECK(snprintf(release_path, sizeof(release_path), "%s/release", root) < (int)sizeof(release_path));
    (void)remove(release_path);
    UmiCtestRunOptions run_options = {0};
    run_options.configuration = "Debug";
    run_options.timeout_ms = 15000U;
    if (strcmp(mode, "observer") == 0 || strcmp(mode, "aliased") == 0 || strcmp(mode, "legacy") == 0 ||
        strcmp(mode, "pre-cancel") == 0)
    {
        UmiTestResult result = {0};
        Observation *capture = calloc(1U, sizeof(*capture));
        CHECK(capture != NULL);
        strcpy(result.name, strcmp(mode, "observer") == 0 ? "output.slow" : "output.pass");
        strcpy(result.test_id, "selected");
        run_options.test_id = result.test_id;
        UmiCancellationToken *token = NULL;
        if (strcmp(mode, "pre-cancel") == 0)
        {
            CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
            umi_cancellation_token_request(token);
            run_options.cancellation = token;
        }
        UmiStatus status = UmiCtestRunObserved(
            root, result.name, &run_options, strcmp(mode, "legacy") == 0 ? NULL : observe, capture, &result);
        CHECK(strcmp(result.test_id, "selected") == 0);
        if (token != NULL)
        {
            CHECK(status == UMI_STATUS_CANCELLED && result.state == UMI_TEST_STATE_CANCELLED);
            CHECK(capture->tail.length == 0U);
            umi_cancellation_token_destroy(token);
        }
        else
        {
            CHECK(status == UMI_STATUS_OK && result.state == UMI_TEST_STATE_PASSED);
            if (strcmp(mode, "observer") == 0)
                CHECK(capture->released && strstr(capture->bytes, "LIVE OUTPUT FINISHED") != NULL);
            if (strcmp(mode, "legacy") == 0)
                CHECK(strstr(result.output, "SMALL OUTPUT END") == NULL);
            if (strcmp(mode, "aliased") == 0)
                CHECK(strstr(capture->bytes, "SMALL OUTPUT END") != NULL);
        }
        free(capture);
        (void)remove(release_path);
        return 0;
    }
    UmiCtestJobRequest requests[2] = {0};
    CHECK(strlen(root) < sizeof(requests[0].build_directory));
    strcpy(requests[0].build_directory, root);
    strcpy(requests[0].configuration, "Debug");
    strcpy(requests[0].test_id, "selected");
    strcpy(requests[0].name, "output.pass");
    requests[0].enabled = 1;
    requests[0].timeout_ms = 15000U;
    if (strcmp(mode, "live") == 0 || strcmp(mode, "cancel") == 0 || strcmp(mode, "timeout") == 0)
        strcpy(requests[0].name, "output.slow");
    if (strcmp(mode, "timeout") == 0)
        requests[0].timeout_ms = 2000U;
    if (strcmp(mode, "large") == 0 || strcmp(mode, "next-attempt") == 0)
        strcpy(requests[0].name, "output.large");
    if (strcmp(mode, "failed") == 0)
        strcpy(requests[0].name, "output.fail");
    if (strcmp(mode, "skipped") == 0)
        strcpy(requests[0].name, "output.skip");
    if (strcmp(mode, "unicode") == 0)
        strcpy(requests[0].name, "output.unicode");
    if (strcmp(mode, "missing") == 0)
        strcpy(requests[0].name, "output.missing");
    if (strcmp(mode, "disabled") == 0)
        requests[0].enabled = 0;
    requests[1] = requests[0];
    strcpy(requests[1].test_id, "second");
    strcpy(requests[1].name, "output.pass");
    UmiCtestJobOptions options = {0};
    options.repeat_count = strcmp(mode, "repeat") == 0 ? 2U : 1U;
    UmiCtestJob *job = NULL;
    CHECK(UmiCtestJobCreate(requests, strcmp(mode, "next-attempt") == 0 ? 2U : 1U, &options, &job) ==
          UMI_STATUS_OK);
    UmiCtestOutputSnapshot *output = calloc(1U, sizeof(*output));
    CHECK(output != NULL);
    CHECK(UmiCtestJobReadOutput(job, output) == UMI_STATUS_OK);
    CHECK(output->task_id != 0U && output->revision == 0U && output->invocation == 0U &&
          output->tail.length == 0U);
    if (strcmp(mode, "invalid") == 0 || strcmp(mode, "created") == 0)
    {
        CHECK(UmiCtestJobReadOutput(NULL, output) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestJobReadOutput(job, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_OK);
        free(output);
        return 0;
    }
    UmiTaskQueue *queue = NULL;
    UmiTaskQueueConfig queue_config = {1U, 2U};
    CHECK(umi_task_queue_create(&queue_config, &queue) == UMI_STATUS_OK);
    CHECK(UmiCtestJobSubmit(job, queue) == UMI_STATUS_OK);
    if (strcmp(mode, "live") == 0 || strcmp(mode, "cancel") == 0)
    {
        wait_for_output(job, output);
        CHECK(!output->attempt_complete && output->result_status == UMI_STATUS_BUSY);
        CHECK(progress(job).completed == 0U && progress(job).state == UMI_TASK_RUNNING);
        CHECK(strcmp(output->test_id, "selected") == 0 && output->attempt == 1U && output->invocation == 1U);
        CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_BUSY);
        if (strcmp(mode, "cancel") == 0)
            CHECK(UmiCtestJobCancel(job) == UMI_STATUS_OK);
        else
            release_child();
    }
    wait_for_job(job);
    CHECK(UmiCtestJobReadOutput(job, output) == UMI_STATUS_OK);
    CHECK(output->attempt_complete && output->capture_status == UMI_STATUS_OK && output->revision >= 2U);
    if (strcmp(mode, "cancel") == 0)
        CHECK(output->result_state == UMI_TEST_STATE_CANCELLED);
    else if (strcmp(mode, "timeout") == 0)
        CHECK(output->result_state == UMI_TEST_STATE_TIMED_OUT);
    else if (strcmp(mode, "failed") == 0)
        CHECK(output->result_state == UMI_TEST_STATE_FAILED);
    else if (strcmp(mode, "skipped") == 0 || strcmp(mode, "disabled") == 0)
        CHECK(output->result_state == UMI_TEST_STATE_SKIPPED);
    else if (strcmp(mode, "missing") == 0)
        CHECK(output->result_status != UMI_STATUS_OK && output->result_state != UMI_TEST_STATE_PASSED);
    else
        CHECK(output->result_state == UMI_TEST_STATE_PASSED && output->result_status == UMI_STATUS_OK);
    if (strcmp(mode, "large") == 0)
    {
        CHECK(output->tail.truncated && output->tail.length == sizeof(output->bytes) - 1U);
        CHECK(output->tail.total_bytes > output->tail.length &&
              strstr(output->bytes, "LARGE OUTPUT END") != NULL);
    }
    if (strcmp(mode, "next-attempt") == 0)
    {
        CHECK(output->invocation == 2U && strcmp(output->test_id, "second") == 0 && !output->tail.truncated);
        CHECK(strstr(output->bytes, "LARGE OUTPUT END") == NULL &&
              strstr(output->bytes, "SMALL OUTPUT END") != NULL);
        UmiTestResult first;
        CHECK(UmiCtestJobResultAt(job, 0U, &first, NULL) == UMI_STATUS_OK);
        CHECK(strstr(first.output, "LARGE OUTPUT END") != NULL);
    }
    if (strcmp(mode, "repeat") == 0)
        CHECK(output->attempt == 2U && output->invocation == 2U && progress(job).completed == 2U);
    if (strcmp(mode, "unicode") == 0)
        CHECK(strstr(output->bytes, "caf\xc3\xa9") != NULL);
    /* Returned snapshots remain valid after both the job and queue are destroyed. */
    uint64_t identity = output->task_id;
    CHECK(UmiCtestJobDestroy(job) == UMI_STATUS_OK);
    CHECK(umi_task_queue_shutdown(queue, 0) == UMI_STATUS_OK);
    umi_task_queue_destroy(queue);
    CHECK(output->task_id == identity && output->attempt_complete);
    free(output);
    (void)remove(release_path);
    return 0;
}
