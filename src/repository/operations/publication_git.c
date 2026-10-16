/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/publication_git.c
 *
 * PURPOSE:
 *   Run bounded Git argument vectors and reuse the existing submodule lock service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "publication_internal.h"
#include "umicom/repository/submodule_lock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CaptureSink {
    UmiPublicationCapture *capture;
    UmiStatus status;
} CaptureSink;

/* NUL-delimited Git records must be captured as bytes, not strlen-truncated
 * diagnostics. Allocation is bounded even for repositories with many files. */
static void capture_bytes(const char *bytes, size_t length, void *context)
{
    CaptureSink *sink = context;
    char *next;
    if (sink->status != UMI_STATUS_OK) return;
    if (length > UMI_PUBLICATION_CAPTURE_LIMIT - sink->capture->size) {
        sink->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return;
    }
    next = realloc(sink->capture->bytes, sink->capture->size + length + 1U);
    if (!next) {
        sink->status = UMI_STATUS_OUT_OF_MEMORY;
        return;
    }
    sink->capture->bytes = next;
    memcpy(next + sink->capture->size, bytes, length);
    sink->capture->size += length;
    next[sink->capture->size] = '\0';
}

UmiStatus umi_publication_native_git(void *context, const char *root,
    const char *const *arguments, size_t count, UmiPublicationCapture *capture)
{
    UmiPublicationState *state = context;
    const UmiToolInfo *git = umi_toolchain_profile_tool(state->profile, UMI_TOOL_GIT);
    UmiProcessRequest request = {0};
    UmiProcessResult *result = calloc(1U, sizeof(*result));
    UmiEnvironmentVariable variables[UMI_PROCESS_MAX_ENVIRONMENT];
    const char *argv[UMI_PROCESS_MAX_ARGUMENTS];
    size_t argc = 0U, envc = 0U;
    CaptureSink sink = {capture, UMI_STATUS_OK};
    UmiStatus status;
    if (!result) return UMI_STATUS_OUT_OF_MEMORY;
    /* Do not let process-global Git redirection silently move index/object
     * writes outside the checkout selected by the caller. */
    const char *redirects[] = {"GIT_DIR", "GIT_WORK_TREE", "GIT_INDEX_FILE",
        "GIT_COMMON_DIR", "GIT_OBJECT_DIRECTORY", "GIT_ALTERNATE_OBJECT_DIRECTORIES"};
    for (size_t i = 0U; i < sizeof(redirects) / sizeof(redirects[0]); ++i) {
        const char *value = getenv(redirects[i]);
        if (value && value[0]) {
            free(result);
            return UMI_STATUS_INVALID_STATE;
        }
    }
    if (!git || git->state != UMI_TOOL_VALIDATED ||
        count > UMI_PROCESS_MAX_ARGUMENTS - 6U) {
        free(result);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    argv[argc++] = "-c"; argv[argc++] = "submodule.recurse=false";
    argv[argc++] = "-c"; argv[argc++] = "core.fsmonitor=false";
    argv[argc++] = "-c"; argv[argc++] = "core.untrackedCache=false";
    for (size_t i = 0U; i < count; ++i) argv[argc++] = arguments[i];
    /* Preserve discovered PATH and tool environment while preventing a watch
     * or unattended invocation from waiting forever for terminal credentials. */
    if (state->environment) {
        if (umi_environment_plan_validate(state->environment) != UMI_STATUS_OK) {
            free(result);
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        const UmiEnvironmentVariable *source =
            umi_environment_plan_variables(state->environment);
        for (size_t i = 0U; i < state->environment->count; ++i) {
            if (strcmp(source[i].name, "GIT_TERMINAL_PROMPT") == 0 ||
                strcmp(source[i].name, "GIT_OPTIONAL_LOCKS") == 0) continue;
            if (envc >= UMI_PROCESS_MAX_ENVIRONMENT - 2U) {
                free(result);
                return UMI_STATUS_CAPACITY_EXCEEDED;
            }
            /* Explicit overlays must obey the same checkout ownership rule
             * as the inherited process environment. */
            for (size_t j = 0U; j < sizeof(redirects) / sizeof(redirects[0]); ++j) {
                if (strcmp(source[i].name, redirects[j]) == 0 && source[i].value[0]) {
                    free(result);
                    return UMI_STATUS_INVALID_STATE;
                }
            }
            variables[envc++] = source[i];
        }
    }
    variables[envc++] = (UmiEnvironmentVariable){"GIT_TERMINAL_PROMPT", "0"};
    variables[envc++] = (UmiEnvironmentVariable){"GIT_OPTIONAL_LOCKS", "0"};
    request.program = git->path;
    request.arguments = argv;
    request.argument_count = argc;
    request.working_directory = root;
    request.environment = variables;
    request.environment_count = envc;
    request.capture_stdout = 1;
    /* Keep diagnostics out of machine-readable stdout. Git may write errors
     * to the owner's terminal; the report never copies remote credentials. */
    request.capture_stderr = 0;
    request.timeout_ms = state->request->timeout_ms;
    request.poll_interval_ms = 20U;
    request.cancellation = state->request->cancellation;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    status = UmiProcessExecuteStreamed(&request, capture_bytes, &sink, result);
    capture->exit_code = result->exit_code;
    if (result->launched && !result->cancelled && !result->timed_out &&
        !result->termination_requested && status == UMI_STATUS_INTERNAL_ERROR)
        status = UMI_STATUS_OK; /* Caller interprets each Git command's exit. */
    if (sink.status != UMI_STATUS_OK) status = sink.status;
    if (!capture->bytes && status == UMI_STATUS_OK) {
        capture->bytes = calloc(1U, 1U);
        if (!capture->bytes) status = UMI_STATUS_OUT_OF_MEMORY;
    }
    free(result);
    return status;
}

UmiStatus umi_publication_native_lock(void *context, const char *root, int dry_run)
{
    UmiPublicationState *state = context;
    UmiRepositorySubmoduleLockRequest request = {root, dry_run};
    UmiRepositorySubmoduleLockReport *report = calloc(1U, sizeof(*report));
    UmiStatus status;
    if (!report) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_repository_submodule_lock(state->profile, state->environment,
        &request, report);
    free(report);
    return status;
}

UmiStatus umi_publication_fail(UmiPublicationState *state, size_t index,
    UmiStatus status, const char *detail)
{
    UmiRepositoryPublicationReport *report = state->report;
    report->status = status;
    (void)snprintf(report->detail, sizeof(report->detail), "%s", detail);
    if (index < report->count) {
        UmiRepositoryPublicationEntry *entry = &report->entries[index];
        if (!entry->blocked) ++report->blocked_count;
        entry->blocked = 1;
        (void)snprintf(entry->detail, sizeof(entry->detail), "%s", detail);
    }
    return status;
}

UmiStatus umi_publication_git(UmiPublicationState *state, size_t index,
    const char *const *args, size_t count, int accept_one,
    UmiPublicationCapture *capture)
{
    UmiStatus status;
    memset(capture, 0, sizeof(*capture));
    capture->exit_code = -1;
    status = state->backend.git(state->backend.context,
        state->report->entries[index].path, args, count, capture);
    if (status == UMI_STATUS_OK && capture->exit_code != 0 &&
        !(accept_one && capture->exit_code == 1)) status = UMI_STATUS_IO_ERROR;
    if (status != UMI_STATUS_OK) {
        char detail[160];
        (void)snprintf(detail, sizeof(detail), "Git %s failed (exit %d); remaining publication stopped.",
            args[0], capture->exit_code);
        free(capture->bytes);
        capture->bytes = NULL;
        return umi_publication_fail(state, index, status, detail);
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_publication_text(UmiPublicationState *state, size_t index,
    const char *const *args, size_t count, char *out, size_t capacity)
{
    UmiPublicationCapture capture;
    UmiStatus status = umi_publication_git(state, index, args, count, 0, &capture);
    if (status != UMI_STATUS_OK) return status;
    while (capture.size && (capture.bytes[capture.size - 1U] == '\n' ||
        capture.bytes[capture.size - 1U] == '\r')) --capture.size;
    if (capture.size >= capacity ||
        memchr(capture.bytes, '\0', capture.size) != NULL) {
        free(capture.bytes);
        return umi_publication_fail(state, index, UMI_STATUS_CAPACITY_EXCEEDED,
            "Git returned an oversized or malformed text record.");
    }
    memcpy(out, capture.bytes, capture.size);
    out[capture.size] = '\0';
    free(capture.bytes);
    return UMI_STATUS_OK;
}
