/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/working_tree_process.c
 * PURPOSE: Collect complete Git status through the shared bounded process stream.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/working_tree.h"
#include "umicom/vcs/revision_changes.h"
#include "revision_changes_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Capture raw bytes independently from the process result's diagnostic text tail. */
typedef struct StatusCapture
{
    char *bytes;
    size_t length;
    size_t capacity;
    UmiStatus status;
} StatusCapture;

/* A stream callback cannot return failure. Remember the first error and discard later chunks. */
static void CaptureStatus(const char *bytes, size_t length, void *context)
{
    StatusCapture *capture = context;
    size_t required;
    if (capture->status != UMI_STATUS_OK || length == 0U)
        return;
    if (bytes == NULL)
    {
        capture->status = UMI_STATUS_IO_ERROR;
        return;
    }
    if (length > UMI_VCS_WORKING_TREE_BYTE_LIMIT - capture->length)
    {
        capture->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    required = capture->length + length;
    if (required > capture->capacity)
    {
        size_t capacity = capture->capacity == 0U ? 4096U : capture->capacity;
        char *grown;
        while (capacity < required && capacity < UMI_VCS_WORKING_TREE_BYTE_LIMIT)
            capacity *= 2U;
        if (capacity > UMI_VCS_WORKING_TREE_BYTE_LIMIT)
            capacity = UMI_VCS_WORKING_TREE_BYTE_LIMIT;
        grown = realloc(capture->bytes, capacity);
        if (grown == NULL)
        {
            capture->status = UMI_STATUS_OUT_OF_MEMORY;
            return;
        }
        capture->bytes = grown;
        capture->capacity = capacity;
    }
    memcpy(capture->bytes + capture->length, bytes, length);
    capture->length = required;
}

/* Keep child processes bounded and invisible, using the shared platform lifetime contract. */
UmiStatus UmiVcsWorkingTreeRead(const UmiVcsWorkingTreeRequest *request,
                                UmiVcsWorkingTree **out_tree)
{
    static const char *const arguments[] = {"--no-optional-locks",
                                            "-c",
                                            "core.fsmonitor=false",
                                            "status",
                                            "--porcelain=v2",
                                            "-z",
                                            "--branch",
                                            "--show-stash",
                                            "--untracked-files=all",
                                            "--ignore-submodules=none",
                                            "--ahead-behind"};
    UmiProcessRequest process = {0};
    UmiProcessResult *result;
    StatusCapture capture = {0};
    UmiVcsWorkingTree *tree = NULL;
    UmiVcsWorkingTreeSummary summary;
    UmiStatus status;
    if (request == NULL || out_tree == NULL || request->repository_root == NULL ||
        request->repository_root[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    result = calloc(1U, sizeof(*result));
    if (result == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    process.program = request->git_program != NULL && request->git_program[0] != '\0'
                          ? request->git_program
                          : "git";
    process.arguments = arguments;
    process.argument_count = sizeof(arguments) / sizeof(arguments[0]);
    process.working_directory = request->repository_root;
    process.capture_stdout = process.capture_stderr = 1;
    process.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    process.timeout_ms = request->timeout_ms != 0U ? request->timeout_ms : 30000U;
    process.poll_interval_ms = 10U;
    process.cancellation = request->cancellation;
    status = UmiProcessExecuteWithLifetime(&process, UMI_PROCESS_LIFETIME_TREE, NULL, CaptureStatus,
                                           &capture, result);
    /* Exit status takes precedence over parsing: error messages must never become a clean result.
     */
    if (status == UMI_STATUS_OK && result->cancelled)
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK && result->timed_out)
        status = UMI_STATUS_TIMEOUT;
    if (status == UMI_STATUS_OK && (!result->launched || result->exit_code != 0))
        status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK)
        status = capture.status;
    if (status == UMI_STATUS_OK)
        status = UmiVcsWorkingTreeParse(capture.bytes, capture.length, &tree);
    /* A successful status --branch reports both headers, including on an unborn branch. */
    if (status == UMI_STATUS_OK)
    {
        status = UmiVcsWorkingTreeDescribe(tree, &summary);
        if (status == UMI_STATUS_OK &&
            (!summary.branch_known || (!summary.commit_known && !summary.unborn)))
            status = UMI_STATUS_PARSE_ERROR;
    }
    free(capture.bytes);
    free(result);
    if (status != UMI_STATUS_OK)
    {
        UmiVcsWorkingTreeDestroy(tree);
        return status;
    }
    *out_tree = tree;
    return UMI_STATUS_OK;
}

/* Both read-only observations use one bounded stream collector and the same
 * cancellation/process-tree ownership. Immutable commit IDs prevent a moving
 * branch name from changing the two endpoints of this comparison. */
UmiStatus UmiVcsRevisionChangesRead(const UmiVcsRevisionChangesRequest *request,
    UmiVcsRevisionChanges **outChanges)
{
    if (request == NULL || outChanges == NULL || *outChanges != NULL ||
        request->repository_root == NULL || request->repository_root[0] == '\0' ||
        request->base_commit == NULL || request->head_commit == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t baseLength = 0U, headLength = 0U;
    while (baseLength <= 64U && request->base_commit[baseLength] != '\0') ++baseLength;
    while (headLength <= 64U && request->head_commit[headLength] != '\0') ++headLength;
    if (baseLength != headLength || !UmiVcsRevisionHex(request->base_commit, baseLength) ||
        !UmiVcsRevisionHex(request->head_commit, headLength)) return UMI_STATUS_INVALID_ARGUMENT;
    char base[80], head[80];
    int baseWritten = snprintf(base, sizeof(base), "%s^{commit}", request->base_commit);
    int headWritten = snprintf(head, sizeof(head), "%s^{commit}", request->head_commit);
    if (baseWritten < 0 || (size_t)baseWritten >= sizeof(base) ||
        headWritten < 0 || (size_t)headWritten >= sizeof(head)) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A partial clone must not fetch during observation, and replacement refs
     * must not change the content represented by the recorded object IDs. */
    const char *arguments[] = {"--no-optional-locks", "--no-lazy-fetch", "--no-replace-objects",
        "--no-pager", "-c", "core.fsmonitor=false",
        "diff", "--raw", "--no-abbrev", "--no-renames", "--no-ext-diff", "--no-textconv",
        "--no-relative", "--ignore-submodules=none", "-z", base, head, "--"};
    UmiProcessRequest process = {0};
    UmiProcessResult *result = calloc(1U, sizeof(*result));
    if (result == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    StatusCapture capture = {0};
    process.program = request->git_program != NULL && request->git_program[0] != '\0'
        ? request->git_program : "git";
    process.arguments = arguments;
    process.argument_count = sizeof(arguments) / sizeof(arguments[0]);
    process.working_directory = request->repository_root;
    process.capture_stdout = process.capture_stderr = 1;
    process.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    process.timeout_ms = request->timeout_ms == 0U ? 30000U : request->timeout_ms;
    process.poll_interval_ms = 10U;
    process.cancellation = request->cancellation;
    UmiStatus status = UmiProcessExecuteWithLifetime(&process, UMI_PROCESS_LIFETIME_TREE,
        NULL, CaptureStatus, &capture, result);
    if (status == UMI_STATUS_OK && result->cancelled) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK && result->timed_out) status = UMI_STATUS_TIMEOUT;
    if (status == UMI_STATUS_OK && (!result->launched || result->exit_code != 0)) status = UMI_STATUS_UNAVAILABLE;
    if (status == UMI_STATUS_OK) status = capture.status;
    if (status == UMI_STATUS_OK) status = UmiVcsRevisionChangesParse(capture.bytes, capture.length, outChanges);
    free(capture.bytes); free(result);
    return status;
}
