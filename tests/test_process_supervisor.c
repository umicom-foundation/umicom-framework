/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_process_supervisor.c
 *
 * PURPOSE:
 *   Verify asynchronous process submission, output capture, completion and
 *   timeout reporting through the shared supervisor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/process_supervisor.h"

#include <assert.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProcessSupervisor *supervisor = NULL;
    UmiProcessRequest request;
    UmiProcessJobId job_id = 0U;
    UmiProcessJobSnapshot snapshot;
#ifdef _WIN32
    const char *arguments[] = {"/C", "echo supervised"};
    const char *slow_arguments[] = {"/C", "ping -n 3 127.0.0.1 >nul"};
#else
    const char *arguments[] = {"-c", "printf supervised"};
    const char *slow_arguments[] = {"-c", "sleep 1"};
#endif

    assert(umi_process_supervisor_create(NULL, &supervisor) == UMI_STATUS_OK);
    (void)memset(&request, 0, sizeof(request));
#ifdef _WIN32
    request.program = "cmd.exe";
#else
    request.program = "/bin/sh";
#endif
    request.arguments = arguments;
    request.argument_count = 2U;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    assert(umi_process_supervisor_submit(supervisor,
                                         "echo",
                                         &request,
                                         &job_id) == UMI_STATUS_OK);
    assert(umi_process_supervisor_wait(supervisor, job_id, 5000U) ==
           UMI_STATUS_OK);
    assert(umi_process_supervisor_snapshot(supervisor,
                                           job_id,
                                           &snapshot) == UMI_STATUS_OK);
    assert(snapshot.state == UMI_PROCESS_JOB_SUCCEEDED);
    assert(strstr(snapshot.output, "supervised") != NULL);

    request.arguments = slow_arguments;
    request.timeout_ms = 50U;
    assert(umi_process_supervisor_submit(supervisor,
                                         "timeout",
                                         &request,
                                         &job_id) == UMI_STATUS_OK);
    assert(umi_process_supervisor_wait(supervisor, job_id, 5000U) ==
           UMI_STATUS_OK);
    assert(umi_process_supervisor_snapshot(supervisor,
                                           job_id,
                                           &snapshot) == UMI_STATUS_OK);
    assert(snapshot.state == UMI_PROCESS_JOB_TIMED_OUT);
    umi_process_supervisor_destroy(supervisor);
    return 0;
}
