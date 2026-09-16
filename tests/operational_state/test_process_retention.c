/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/operational_state/test_process_retention.c
 * PURPOSE: Exercise retained results, concurrent waiters and bounded job reuse.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/clock.h"
#include "umicom/platform/filesystem.h"
#include "umicom/platform/process_supervisor.h"
#include "umicom/platform/threading.h"
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return EXIT_FAILURE; } } while (0)

typedef struct Waiter { UmiProcessSupervisor *supervisor; UmiProcessJobId job; UmiStatus result; } Waiter;
static int Await(void *context)
{
    Waiter *waiter = context;
    waiter->result = umi_process_supervisor_wait(waiter->supervisor, waiter->job, 10000U);
    return waiter->result == UMI_STATUS_OK ? 0 : 1;
}
static UmiProcessRequest Request(const char *program, const char *const *arguments, size_t count)
{
    UmiProcessRequest request = {0};
    request.program = program; request.arguments = arguments; request.argument_count = count;
    request.capture_stdout = 1; request.capture_stderr = 1;
    request.timeout_ms = 10000U;
    return request;
}
static int Reuse(const char *program)
{
    UmiProcessSupervisor *supervisor = NULL;
    UmiProcessSupervisorConfig config = {2U};
    UmiProcessJobSnapshot snapshot;
    UmiProcessJobId first = 0U, id = 0U, previous = 0U;
    const char *arguments[] = {"child"};
    UmiProcessRequest request = Request(program, arguments, 1U);
    CHECK(umi_process_supervisor_create(&config, &supervisor) == UMI_STATUS_OK);
    for (unsigned i = 0U; i < 96U; ++i) {
        CHECK(umi_process_supervisor_submit(supervisor, "Umicom harmless child", &request, &id) == UMI_STATUS_OK);
        CHECK(id > previous); previous = id;
        if (i == 0U) first = id;
        CHECK(umi_process_supervisor_wait(supervisor, id, 10000U) == UMI_STATUS_OK);
        CHECK(umi_process_supervisor_wait(supervisor, id, 10000U) == UMI_STATUS_OK);
        CHECK(umi_process_supervisor_snapshot(supervisor, id, &snapshot) == UMI_STATUS_OK);
        CHECK(snapshot.state == UMI_PROCESS_JOB_SUCCEEDED && snapshot.exit_code == 0);
        CHECK(strstr(snapshot.output, "Umicom child completed") != NULL);
        CHECK(umi_process_supervisor_at(supervisor, 0U, &snapshot) == UMI_STATUS_OK && snapshot.job_id == id);
        CHECK(UmiProcessSupervisorReleaseJob(supervisor, id) == UMI_STATUS_OK);
        CHECK(umi_process_supervisor_snapshot(supervisor, id, &snapshot) == UMI_STATUS_NOT_FOUND);
        CHECK(umi_process_supervisor_at(supervisor, 0U, &snapshot) == UMI_STATUS_NOT_FOUND);
    }
    UmiProcessSupervisorStats stats = umi_process_supervisor_stats(supervisor);
    CHECK(stats.jobs == 0U && stats.running == 0U && stats.submitted == 96U && stats.succeeded == 96U);
    CHECK(UmiProcessSupervisorReleaseJob(supervisor, first) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiProcessSupervisorReleaseJob(supervisor, 0U) == UMI_STATUS_INVALID_ARGUMENT);
    umi_process_supervisor_destroy(supervisor);
    return EXIT_SUCCESS;
}
static int Concurrency(const char *program)
{
    UmiProcessSupervisor *supervisor = NULL;
    UmiProcessSupervisorConfig config = {2U};
    UmiProcessJobId job;
    UmiThread *threads[2] = {NULL, NULL};
    Waiter waiters[2];
    UmiClock clock = umi_clock_system();
    char gate[256];
    (void)snprintf(gate, sizeof(gate), "process-gate-%" PRIu64, clock.wall_nanoseconds(&clock));
    CHECK(!umi_fs_exists(gate));
    const char *arguments[] = {"gate-child", gate};
    UmiProcessRequest request = Request(program, arguments, 2U);
    UmiProcessJobSnapshot snapshot;
    CHECK(umi_process_supervisor_create(&config, &supervisor) == UMI_STATUS_OK);
    CHECK(umi_process_supervisor_submit(supervisor, "live", &request, &job) == UMI_STATUS_OK);
    CHECK(UmiProcessSupervisorReleaseJob(supervisor, job) == UMI_STATUS_BUSY);
    CHECK(umi_process_supervisor_wait(supervisor, job, 1U) == UMI_STATUS_TIMEOUT);
    for (unsigned i = 0U; i < 2U; ++i) {
        waiters[i] = (Waiter){supervisor, job, UMI_STATUS_INTERNAL_ERROR};
        CHECK(umi_thread_start(Await, &waiters[i], &threads[i]) == UMI_STATUS_OK);
    }
    for (unsigned i = 0U; i < 5000U; ++i) {
        CHECK(umi_process_supervisor_snapshot(supervisor, job, &snapshot) == UMI_STATUS_OK);
        if (strstr(snapshot.output, "started") != NULL) break;
        umi_thread_sleep_ms(1U);
    }
    /* The child cannot finish until the parent creates its test-owned gate.
     * Live output must remain visible, with no unsynchronised result reads. */
    CHECK(snapshot.state == UMI_PROCESS_JOB_RUNNING);
    CHECK(strstr(snapshot.output, "started") != NULL);
    CHECK(UmiProcessSupervisorReleaseJob(supervisor, job) == UMI_STATUS_BUSY);
    CHECK(umi_fs_write_text(gate, "finish") == UMI_STATUS_OK);
    for (unsigned i = 0U; i < 2U; ++i) {
        int code = -1;
        CHECK(umi_thread_join(threads[i], &code) == UMI_STATUS_OK);
        CHECK(code == 0 && waiters[i].result == UMI_STATUS_OK);
        umi_thread_destroy(threads[i]);
    }
    CHECK(umi_process_supervisor_snapshot(supervisor, job, &snapshot) == UMI_STATUS_OK);
    CHECK(strstr(snapshot.output, "finished") != NULL);
    CHECK(umi_fs_remove_tree(gate) == UMI_STATUS_OK);
    CHECK(UmiProcessSupervisorReleaseJob(supervisor, job) == UMI_STATUS_OK);
    umi_process_supervisor_destroy(supervisor);
    return EXIT_SUCCESS;
}
static int Shutdown(const char *program)
{
    UmiProcessSupervisor *supervisor = NULL;
    UmiProcessSupervisorConfig config = {2U};
    UmiProcessJobId job, unchanged = 1234U;
    UmiProcessJobSnapshot snapshot;
    const char *arguments[] = {"long-child"};
    UmiProcessRequest request = Request(program, arguments, 1U);
    CHECK(umi_process_supervisor_create(&config, &supervisor) == UMI_STATUS_OK);
    CHECK(umi_process_supervisor_submit(supervisor, "cancel on shutdown", &request, &job) == UMI_STATUS_OK);
    CHECK(umi_process_supervisor_shutdown(supervisor) == UMI_STATUS_OK);
    CHECK(umi_process_supervisor_snapshot(supervisor, job, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.state == UMI_PROCESS_JOB_CANCELLED);
    CHECK(umi_process_supervisor_submit(supervisor, "rejected", &request, &unchanged) == UMI_STATUS_INVALID_STATE);
    CHECK(unchanged == 1234U);
    CHECK(umi_process_supervisor_shutdown(supervisor) == UMI_STATUS_OK);
    umi_process_supervisor_destroy(supervisor);
    return EXIT_SUCCESS;
}
int main(int argc, char **argv)
{
    char program[UMI_PATH_CAPACITY];
    if (argc == 3 && strcmp(argv[1], "gate-child") == 0) {
        puts("started"); fflush(stdout);
        for (unsigned i = 0U; i < 10000U; ++i) {
            if (umi_fs_exists(argv[2])) { puts("finished"); return 0; }
            umi_thread_sleep_ms(1U);
        }
        return 2;
    }
    if (argc != 2) return EXIT_FAILURE;
    if (strcmp(argv[1], "child") == 0) { puts("Umicom child completed"); return 0; }
    if (strcmp(argv[1], "long-child") == 0) { umi_thread_sleep_ms(10000U); return 0; }
    CHECK(umi_fs_executable_path(program, sizeof(program)) == UMI_STATUS_OK);
    if (strcmp(argv[1], "reuse") == 0) return Reuse(program);
    if (strcmp(argv[1], "concurrency") == 0) return Concurrency(program);
    if (strcmp(argv[1], "shutdown") == 0) return Shutdown(program);
    return EXIT_FAILURE;
}
