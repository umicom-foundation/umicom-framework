/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_process_supervisor_lifecycle.c
 *
 * PURPOSE:
 *   Verify checked supervisor release and retry after native lifecycle failures.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "support/assertions.h"
#include "umicom/platform/process_supervisor.h"
#include "umicom/platform/threading.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef UMICOM_SUPERVISOR_LIFECYCLE_FAULTS
UmiStatus __real_umi_thread_join(UmiThread *thread, int *result);
UmiStatus __real_UmiThreadRelease(UmiThread **thread);
UmiStatus __wrap_umi_thread_join(UmiThread *thread, int *result);
UmiStatus __wrap_UmiThreadRelease(UmiThread **thread);
static int failJoin;
static int failRelease;
static int unavailableAfterJoin;
static unsigned joins;
UmiStatus __wrap_umi_thread_join(UmiThread *thread, int *result)
{
    ++joins;
    if (failJoin) {
        failJoin = 0;
        return UMI_STATUS_INTERNAL_ERROR;
    }
    UmiStatus status = __real_umi_thread_join(thread, result);
    /* A native join can complete while its callback result is unavailable.
     * Consume the real join first so retry would expose a double-join bug. */
    if (status == UMI_STATUS_OK && unavailableAfterJoin) {
        unavailableAfterJoin = 0;
        return UMI_STATUS_UNAVAILABLE;
    }
    return status;
}
UmiStatus __wrap_UmiThreadRelease(UmiThread **thread)
{
    if (failRelease) {
        failRelease = 0;
        return UMI_STATUS_INTERNAL_ERROR;
    }
    return __real_UmiThreadRelease(thread);
}
#endif

static void WaitForTerminal(UmiProcessSupervisor *supervisor, UmiProcessJobId id)
{
    /* Observe terminal publication without consuming the join obligation.
     * This lets the failure fixture target the subsequent native join. */
    for (unsigned attempt = 0U; attempt < 10000U; ++attempt) {
        UmiProcessJobSnapshot snapshot;
        assert(umi_process_supervisor_snapshot(supervisor, id, &snapshot) == UMI_STATUS_OK);
        if (snapshot.state != UMI_PROCESS_JOB_CREATED &&
            snapshot.state != UMI_PROCESS_JOB_RUNNING) {
            assert(snapshot.state == UMI_PROCESS_JOB_SUCCEEDED);
            assert(snapshot.exit_code == 0);
            assert(strstr(snapshot.output, "supervisor-lifecycle-child") != NULL);
            return;
        }
        umi_thread_sleep_ms(1U);
    }
    assert(!"Process worker did not publish terminal state");
}

int main(int argc, char **argv)
{
    /* The fixture runs this same known executable; no shell, network command
     * or external workload is needed to test process ownership. */
    if (argc == 2 && strcmp(argv[1], "--child") == 0) {
        puts("supervisor-lifecycle-child");
        return 0;
    }
    assert(argc == 3);
    UmiProcessSupervisor *supervisor = NULL;
    assert(UmiProcessSupervisorRelease(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    assert(UmiProcessSupervisorRelease(&supervisor) == UMI_STATUS_OK);
    assert(umi_process_supervisor_create(NULL, &supervisor) == UMI_STATUS_OK);
    if (strcmp(argv[1], "empty") == 0) {
        assert(UmiProcessSupervisorRelease(&supervisor) == UMI_STATUS_OK);
        assert(supervisor == NULL);
        return 0;
    }
    const char *arguments[] = {"--child"};
    UmiProcessRequest request = {0};
    request.program = argv[2];
    request.arguments = arguments;
    request.argument_count = 1U;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = 5000U;
    UmiProcessJobId id = 0U;
    assert(umi_process_supervisor_submit(supervisor, "lifecycle child", &request, &id) == UMI_STATUS_OK);
    WaitForTerminal(supervisor, id);
#ifdef UMICOM_SUPERVISOR_LIFECYCLE_FAULTS
    UmiProcessSupervisor *owned = supervisor;
    if (strcmp(argv[1], "shutdown-retry") == 0) {
        failJoin = 1;
        assert(umi_process_supervisor_shutdown(supervisor) == UMI_STATUS_INTERNAL_ERROR);
        assert(joins == 1U);
        assert(umi_process_supervisor_shutdown(supervisor) == UMI_STATUS_OK);
        assert(joins == 2U);
    } else if (strcmp(argv[1], "release-retry") == 0) {
        failRelease = 1;
        assert(UmiProcessSupervisorRelease(&supervisor) == UMI_STATUS_INTERNAL_ERROR);
        assert(supervisor == owned && joins == 1U);
    } else if (strcmp(argv[1], "legacy-retry") == 0) {
        failJoin = 1;
        umi_process_supervisor_destroy(supervisor);
        assert(umi_process_supervisor_stats(supervisor).jobs == 1U);
        assert(joins == 1U);
    } else if (strcmp(argv[1], "joined-without-result") == 0) {
        unavailableAfterJoin = 1;
        assert(UmiProcessSupervisorRelease(&supervisor) == UMI_STATUS_UNAVAILABLE);
        assert(supervisor == owned && joins == 1U);
    } else if (strcmp(argv[1], "job-release-retry") == 0) {
        failRelease = 1;
        assert(UmiProcessSupervisorReleaseJob(supervisor, id) == UMI_STATUS_INTERNAL_ERROR);
        assert(umi_process_supervisor_stats(supervisor).jobs == 1U && joins == 1U);
        assert(UmiProcessSupervisorReleaseJob(supervisor, id) == UMI_STATUS_OK);
        assert(umi_process_supervisor_stats(supervisor).jobs == 0U && joins == 1U);
    } else {
        assert(!"Unknown supervisor fault scenario");
    }
    const unsigned expectedJoins = strcmp(argv[1], "shutdown-retry") == 0 ||
        strcmp(argv[1], "legacy-retry") == 0 ? 2U : 1U;
#else
    assert(strcmp(argv[1], "completed") == 0);
    assert(umi_process_supervisor_wait(supervisor, id, 5000U) == UMI_STATUS_OK);
    assert(umi_process_supervisor_shutdown(supervisor) == UMI_STATUS_OK);
    assert(umi_process_supervisor_shutdown(supervisor) == UMI_STATUS_OK);
#endif
    assert(UmiProcessSupervisorRelease(&supervisor) == UMI_STATUS_OK && supervisor == NULL);
    assert(UmiProcessSupervisorRelease(&supervisor) == UMI_STATUS_OK);
#ifdef UMICOM_SUPERVISOR_LIFECYCLE_FAULTS
    assert(joins == expectedJoins);
#endif
    return 0;
}
