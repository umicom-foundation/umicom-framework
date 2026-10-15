/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_git_capture.c
 * PURPOSE: Verify failed and incomplete Git captures remain distinguishable from successful snapshots.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/repository/git_command.h"
#include "umicom/platform/process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *scenario;
static unsigned calls;
static int request_valid;
static UmiStatus CaptureProcess(const UmiProcessRequest *request, UmiProcessResult *result)
{
    ++calls;
    request_valid = strcmp(request->program, "review-git") == 0 &&
        strcmp(request->working_directory, "review-root") == 0 &&
        request->argument_count == 2U &&
        strcmp(request->arguments[1], "--porcelain=v2") == 0 &&
        request->timeout_ms == 321U && request->window_mode == UMI_PROCESS_WINDOW_HIDDEN &&
        request->capture_stdout && request->capture_stderr;
    result->launched = 1;
    result->exit_code = 0;
    strcpy(result->output, "captured evidence");
    if (strcmp(scenario, "nonzero") == 0) { result->exit_code = 128; return UMI_STATUS_INTERNAL_ERROR; }
    if (strcmp(scenario, "exit_only") == 0) result->exit_code = 1;
    if (strcmp(scenario, "timeout") == 0) { result->timed_out = 1; return UMI_STATUS_TIMEOUT; }
    if (strcmp(scenario, "cancel") == 0) { result->cancelled = 1; return UMI_STATUS_CANCELLED; }
    if (strcmp(scenario, "termination") == 0) result->termination_requested = 1;
    if (strcmp(scenario, "truncated") == 0) result->output_truncated = 1;
    if (strcmp(scenario, "unterminated") == 0) memset(result->output, 'x', sizeof result->output);
    if (strcmp(scenario, "launch") == 0) {
        result->launched = 0; result->exit_code = -1; return UMI_STATUS_NOT_FOUND;
    }
    return UMI_STATUS_OK;
}
/* Inject only the native process boundary. The real capture code still owns
 * allocation, bounded copying, evidence flags and status precedence. */
#define umi_process_execute CaptureProcess
#define umi_repository_git_command_execute CaptureGit
#include "../../src/repository/inspection/git_command.c"
#undef umi_process_execute
#undef umi_repository_git_command_execute
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

int main(int argc, char **argv)
{
    int failed = 0;
    UmiRepositoryGitCommandResult *result = malloc(sizeof *result);
    CHECK(result != NULL && argc == 2);
    scenario = argv[1];
    UmiRepositoryInspectionContext context = {"review-git", "review-root", 321U};
    const char *arguments[] = {"status", "--porcelain=v2"};
    if (strcmp(scenario, "invalid") == 0) {
        memset(result, 'x', sizeof *result);
        CHECK(CaptureGit(NULL, arguments, 2U, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(result->exit_code == -1 && result->launched == 0 && result->output[0] == '\0');
        CHECK(CaptureGit(&context, arguments, UMI_PROCESS_MAX_ARGUMENTS + 1U, result) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(CaptureGit(&context, NULL, 2U, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(CaptureGit(&context, arguments, 0U, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(CaptureGit(&context, arguments, 2U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        const char *invalid[] = {"status", NULL};
        CHECK(CaptureGit(&context, invalid, 2U, result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(calls == 0U);
        goto cleanup;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(scenario, "nonzero") == 0) expected = UMI_STATUS_INTERNAL_ERROR;
    if (strcmp(scenario, "exit_only") == 0) expected = UMI_STATUS_IO_ERROR;
    if (strcmp(scenario, "timeout") == 0) expected = UMI_STATUS_TIMEOUT;
    if (strcmp(scenario, "cancel") == 0 || strcmp(scenario, "termination") == 0) expected = UMI_STATUS_CANCELLED;
    if (strcmp(scenario, "truncated") == 0 || strcmp(scenario, "unterminated") == 0) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(scenario, "launch") == 0) expected = UMI_STATUS_NOT_FOUND;
    CHECK(CaptureGit(&context, arguments, 2U, result) == expected);
    CHECK(calls == 1U && request_valid);
    if (strcmp(scenario, "unterminated") == 0) CHECK(result->output[0] == '\0');
    else CHECK(strcmp(result->output, "captured evidence") == 0);
    CHECK(result->timed_out == (strcmp(scenario, "timeout") == 0));
    CHECK(result->cancelled == (strcmp(scenario, "cancel") == 0));
    CHECK(result->termination_requested == (strcmp(scenario, "termination") == 0));
    CHECK(result->output_truncated == (strcmp(scenario, "truncated") == 0 || strcmp(scenario, "unterminated") == 0));
    if (strcmp(scenario, "nonzero") == 0) CHECK(result->exit_code == 128);
    if (strcmp(scenario, "launch") == 0) CHECK(!result->launched && result->exit_code == -1);
cleanup:
    free(result);
    return failed;
}
