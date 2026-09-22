/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/development/native_tool/main.c
 * PURPOSE: Submit a compiler-tool probe to the shared asynchronous supervisor.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/platform/process_supervisor.h"
#include <stdio.h>

int main(void)
{
    UmiProcessSupervisor *supervisor = NULL;
    UmiProcessJobId job = 0U;
    UmiProcessRequest request = {0};
    UmiProcessJobSnapshot snapshot = {0};
    const char *arguments[] = {"--version"};
    /* A configured tool catalogue should provide an absolute executable path.
     * The example uses CMake from the prepared development environment. */
    request.program = "cmake";
    request.arguments = arguments;
    request.argument_count = sizeof(arguments) / sizeof(arguments[0]);
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = 10000U;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    UmiStatus status = umi_process_supervisor_create(NULL, &supervisor);
    if (status == UMI_STATUS_OK)
        status = UmiProcessSupervisorSubmitWithLifetime(supervisor,
            "CMake tool information", &request, UMI_PROCESS_LIFETIME_TREE, &job);
    /* This console example waits. A GTK frontend polls snapshot() from its
     * refresh loop and calls cancel() from its Stop action instead. */
    if (status == UMI_STATUS_OK) status = umi_process_supervisor_wait(supervisor, job, 15000U);
    if (status == UMI_STATUS_OK) status = umi_process_supervisor_snapshot(supervisor, job, &snapshot);
    if (status == UMI_STATUS_OK) {
        fputs(snapshot.output, stdout);
        if (snapshot.output_truncated) fputs("[The compact view is truncated.]\n", stderr);
        if (snapshot.state != UMI_PROCESS_JOB_SUCCEEDED || snapshot.exit_code != 0)
            status = UMI_STATUS_INTERNAL_ERROR;
    }
    if (status != UMI_STATUS_OK) fprintf(stderr, "Tool request failed: %d\n", (int)status);
    umi_process_supervisor_destroy(supervisor);
    return status == UMI_STATUS_OK ? 0 : 1;
}
