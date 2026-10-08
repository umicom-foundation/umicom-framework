/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/fixture.h
 * PURPOSE: Provide a controlled immutable job for archive failure and recovery coverage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_ARCHIVE_FIXTURE_H
#define UMICOM_TEST_ARCHIVE_FIXTURE_H
#include "../../src/testing/archive_internal.h"
#include "umicom/testing/archive_write.h"
#include "umicom/platform/threading.h"
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef CHECK
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
#endif
struct UmiCtestJob
{
    UmiCtestJobSnapshot snapshot;
    UmiCtestJobPlanSnapshot plan;
    UmiCtestJobRequest requests[2];
    UmiTestResult results[4];
    UmiCancellationToken *cancel;
    size_t cancel_after;
    atomic_int block;
    atomic_int entered;
};
static inline UmiCtestJob *FixtureJob(void)
{
    UmiCtestJob *job = calloc(1U, sizeof(*job));
    CHECK(job != NULL);
    atomic_init(&job->block, 0);
    atomic_init(&job->entered, 0);
    job->plan.request_count = 2;
    job->plan.repeat_count = 2;
    job->snapshot.task_id = 91;
    job->snapshot.state = UMI_TASK_FAILED;
    job->snapshot.status = UMI_STATUS_IO_ERROR;
    job->snapshot.first_error = UMI_STATUS_IO_ERROR;
    job->snapshot.planned = 4;
    job->snapshot.completed = 4;
    job->snapshot.passed = 1;
    job->snapshot.skipped = 1;
    job->snapshot.failed = 1;
    job->snapshot.timed_out = 1;
    job->snapshot.duration_ms = 40;
    for (size_t i = 0; i < 2U; ++i)
    {
        (void)snprintf(job->requests[i].test_id, sizeof(job->requests[i].test_id), "test.%zu", i);
        (void)snprintf(job->requests[i].name, sizeof(job->requests[i].name), "caf\xc3\xa9 | case %zu", i);
        strcpy(job->requests[i].build_directory, "/fixture/build");
        strcpy(job->requests[i].configuration, "Debug");
        job->requests[i].timeout_ms = 100;
        job->requests[i].enabled = 1;
    }
    const UmiTestState states[] = {UMI_TEST_STATE_PASSED, UMI_TEST_STATE_SKIPPED, UMI_TEST_STATE_FAILED,
                                   UMI_TEST_STATE_TIMED_OUT};
    for (size_t i = 0; i < 4U; ++i)
    {
        strcpy(job->results[i].test_id, job->requests[i % 2U].test_id);
        strcpy(job->results[i].name, job->requests[i % 2U].name);
        job->results[i].state = states[i];
        job->results[i].duration_ms = 10;
        job->results[i].status = i == 2U ? UMI_STATUS_IO_ERROR : i == 3U ? UMI_STATUS_TIMEOUT : UMI_STATUS_OK;
        job->results[i].exit_code = i < 2U ? 0 : -1;
        strcpy(job->results[i].output, "private diagnostic | caf\xc3\xa9\n");
    }
    return job;
}
static inline UmiTestArchiveOrigin FixtureOrigin(bool retain)
{
    UmiTestArchiveOrigin origin = {0};
    strcpy(origin.source_root, "/fixture/source");
    strcpy(origin.source_revision, "captured-by-fixture");
    origin.workspace_generation = 7;
    origin.retain_output = retain;
    return origin;
}
/* This executable supplies only copied job observations. It never invokes
 * CTest or a compiler. Worker tests use atomics to control an in-flight read. */
UmiStatus UmiCtestJobGetSnapshot(UmiCtestJob *job, UmiCtestJobSnapshot *out)
{
    if (!job || !out)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = job->snapshot;
    return UMI_STATUS_OK;
}
UmiStatus UmiCtestJobReadPlan(UmiCtestJob *job, UmiCtestJobPlanSnapshot *out)
{
    if (!job || !out)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = job->plan;
    return UMI_STATUS_OK;
}
UmiStatus UmiCtestJobRequestAt(UmiCtestJob *job, size_t index, UmiCtestJobRequest *out)
{
    if (!job || !out)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= 2U)
        return UMI_STATUS_NOT_FOUND;
    *out = job->requests[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiCtestJobResultAt(UmiCtestJob *job, size_t index, UmiTestResult *out, uint32_t *attempt)
{
    if (!job || !out || !attempt)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= job->snapshot.completed)
        return UMI_STATUS_NOT_FOUND;
    atomic_store(&job->entered, 1);
    while (atomic_load(&job->block))
        umi_thread_sleep_ms(1);
    *out = job->results[index];
    *attempt = (uint32_t)(index / 2U + 1U);
    if (job->cancel != NULL && index == job->cancel_after)
        umi_cancellation_token_request(job->cancel);
    return UMI_STATUS_OK;
}
static inline void FixtureEmpty(UmiTestArchive *archive)
{
    UmiTestArchiveCatalog *catalog = calloc(1, sizeof(*catalog));
    CHECK(catalog != NULL);
    CHECK(UmiTestArchiveList(archive, catalog) == UMI_STATUS_OK && catalog->count == 0);
    free(catalog);
}
#endif
