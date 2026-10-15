/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/git_command.c
 *
 * PURPOSE:
 *   Execute all repository Git probes through the existing safe process abstraction.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/git_command.h"

#include <string.h>
#include <stdlib.h>
#include "umicom/platform/process.h"

/* Execute one read-only or explicitly selected Git operation using argument vectors. */

/* Preserve the previous process capture for review. It returned before copying
 * failed process evidence and could treat truncated output as a whole snapshot.
 * The replacement keeps a bounded heap buffer and reports incomplete evidence. */
#if 0
UmiStatus umi_repository_git_command_execute(
    const UmiRepositoryInspectionContext *context,
    const char *const *arguments,
    size_t argument_count,
    UmiRepositoryGitCommandResult *out_result)
{
    UmiProcessRequest request;
    UmiProcessResult result;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (context == NULL || arguments == NULL || argument_count == 0U ||
        out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (argument_count > UMI_PROCESS_MAX_ARGUMENTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)memset(&request, 0, sizeof(request));
    (void)memset(&result, 0, sizeof(result));
    (void)memset(out_result, 0, sizeof(*out_result));
    request.program =
        (context->git_program != NULL && context->git_program[0] != '\0')
            ? context->git_program
            : "git";
    request.arguments = arguments;
    request.argument_count = argument_count;
    request.working_directory = context->repository_root;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = context->timeout_ms;
    request.poll_interval_ms = 10U;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;

    status = umi_process_execute(&request, &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_result->exit_code = result.exit_code;
    out_result->launched = result.launched;
    out_result->timed_out = result.timed_out;
    out_result->cancelled = result.cancelled;
    return umi_repository_inspection_copy_text(
        out_result->output, sizeof(out_result->output), result.output);
}
#endif

UmiStatus umi_repository_git_command_execute(
    const UmiRepositoryInspectionContext *context,
    const char *const *arguments,
    size_t argument_count,
    UmiRepositoryGitCommandResult *out_result)
{
    if (out_result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_result, 0, sizeof *out_result);
    out_result->exit_code = -1;
    if (context == NULL || context->repository_root == NULL ||
        context->repository_root[0] == '\0' || arguments == NULL || argument_count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (argument_count > UMI_PROCESS_MAX_ARGUMENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < argument_count; ++i)
        if (arguments[i] == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /* Keep the 64 KiB process capture off a worker's stack. The caller owns its
     * report independently and can retain diagnostics after the temporary is freed. */
    UmiProcessResult *result = calloc(1U, sizeof *result);
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    result->exit_code = -1;
    UmiProcessRequest request = {0};
    request.program = context->git_program != NULL && context->git_program[0] != '\0'
        ? context->git_program : "git";
    request.arguments = arguments;
    request.argument_count = argument_count;
    request.working_directory = context->repository_root;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.timeout_ms = context->timeout_ms;
    request.poll_interval_ms = 10U;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    UmiStatus status = umi_process_execute(&request, result);

    /* Failure is evidence too. Preserve the original process status while making
     * its child exit, cancellation and partial transcript available to the caller. */
    out_result->exit_code = result->exit_code;
    out_result->launched = result->launched;
    out_result->timed_out = result->timed_out;
    out_result->cancelled = result->cancelled;
    out_result->termination_requested = result->termination_requested;
    out_result->output_truncated = result->output_truncated;
    const char *end = memchr(result->output, '\0', sizeof result->output);
    UmiStatus copy_status = UMI_STATUS_OK;
    if (end == NULL) {
        out_result->output_truncated = 1;
        copy_status = UMI_STATUS_CAPACITY_EXCEEDED;
    } else {
        copy_status = umi_repository_inspection_copy_text(
            out_result->output, sizeof out_result->output, result->output);
        if (copy_status != UMI_STATUS_OK) out_result->output_truncated = 1;
    }
    free(result);
    if (status != UMI_STATUS_OK) return status;
    if (out_result->timed_out) return UMI_STATUS_TIMEOUT;
    if (out_result->cancelled || out_result->termination_requested) return UMI_STATUS_CANCELLED;
    if (!out_result->launched || out_result->exit_code != 0) return UMI_STATUS_IO_ERROR;
    if (copy_status != UMI_STATUS_OK) return copy_status;
    if (out_result->output_truncated) return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
