/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/git_capture.c
 * PURPOSE: Preserve bounded binary Git output and process failure evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/repository/git_capture.h"
#include "umicom/platform/process.h"
#include <stdlib.h>
#include <string.h>

typedef struct GitCaptureSink {
    UmiRepositoryGitCaptureResult *result;
    size_t capacity;
} GitCaptureSink;

/* The raw observer runs on the invoking worker. It copies exact bytes before
 * the process runner reduces its separate diagnostic tail. A full sink records
 * incomplete evidence and ignores later chunks without overrunning its bound. */
static void GitCaptureBytes(const char *bytes, size_t length, void *context)
{
    GitCaptureSink *sink = context;
    UmiRepositoryGitCaptureResult *result = sink->result;
    if (result->output_truncated || length == 0U) return;
    if (bytes == NULL) { result->output_truncated = 1; return; }
    size_t remaining = sink->capacity - result->length;
    size_t count = length < remaining ? length : remaining;
    if (count != 0U) memcpy(result->bytes + result->length, bytes, count);
    result->length += count;
    result->bytes[result->length] = 0U;
    if (count != length) result->output_truncated = 1;
}

void UmiRepositoryGitCaptureRelease(UmiRepositoryGitCaptureResult *result)
{
    if (result == NULL) return;
    free(result->bytes);
    memset(result, 0, sizeof *result);
    result->exit_code = -1;
}

UmiStatus UmiRepositoryGitCapture(const UmiRepositoryInspectionContext *context,
    const char *const *arguments, size_t argumentCount, size_t maximumBytes,
    const UmiCancellationToken *cancellation, UmiRepositoryGitCaptureResult *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    out->exit_code = -1;
    out->status = UMI_STATUS_INVALID_ARGUMENT;
    if (context == NULL || context->repository_root == NULL ||
        context->repository_root[0] == '\0' || arguments == NULL ||
        argumentCount == 0U || maximumBytes == 0U) return out->status;
    if (argumentCount > UMI_PROCESS_MAX_ARGUMENTS || maximumBytes > UMI_REPOSITORY_GIT_CAPTURE_LIMIT) {
        out->status = UMI_STATUS_CAPACITY_EXCEEDED; return out->status;
    }
    for (size_t i = 0U; i < argumentCount; ++i)
        if (arguments[i] == NULL) return out->status;
    /* Allocate the full requested bound before starting a child. Failure cannot
     * leave an executing process whose output this operation cannot retain. */
    UmiProcessResult *process = calloc(1U, sizeof *process);
    out->bytes = malloc(maximumBytes + 1U);
    if (process == NULL || out->bytes == NULL) {
        free(process); UmiRepositoryGitCaptureRelease(out);
        out->status = UMI_STATUS_OUT_OF_MEMORY; return out->status;
    }
    out->bytes[0] = 0U;
    process->exit_code = -1;
    UmiProcessRequest request = {
        .program = context->git_program != NULL && context->git_program[0] != '\0'
            ? context->git_program : "git",
        .arguments = arguments, .argument_count = argumentCount,
        .working_directory = context->repository_root,
        .capture_stdout = 1, .capture_stderr = 1,
        .timeout_ms = context->timeout_ms, .poll_interval_ms = 10U,
        .cancellation = cancellation, .window_mode = UMI_PROCESS_WINDOW_HIDDEN
    };
    GitCaptureSink sink = {out, maximumBytes};
    UmiStatus status = UmiProcessExecuteWithLifetime(&request, UMI_PROCESS_LIFETIME_TREE,
        NULL, GitCaptureBytes, &sink, process);
    out->exit_code = process->exit_code;
    out->launched = process->launched;
    out->timed_out = process->timed_out;
    out->cancelled = process->cancelled;
    out->termination_requested = process->termination_requested;
    /* process->output_truncated describes only its diagnostic tail. The owned
     * byte sink has its own bound and can legitimately contain more than 64 KiB. */
    free(process);
    if (status == UMI_STATUS_OK) {
        if (out->timed_out) status = UMI_STATUS_TIMEOUT;
        else if (out->cancelled || out->termination_requested) status = UMI_STATUS_CANCELLED;
        else if (out->output_truncated) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else if (!out->launched || out->exit_code != 0) status = UMI_STATUS_IO_ERROR;
    }
    out->status = status;
    return status;
}
