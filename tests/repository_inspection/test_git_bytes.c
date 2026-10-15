/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_git_bytes.c
 * PURPOSE: Exercise exact binary capture, capture bounds and failed Git process evidence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/repository/git_capture.h"
#include "umicom/platform/process.h"
#include <stdio.h>
#include <string.h>
static const char *scenario;
static unsigned calls;
static int validRequest;
static UmiStatus FakeExecute(const UmiProcessRequest *request, UmiProcessLifetime lifetime,
    UmiProcessResultObserver observer, UmiProcessOutputObserver raw, void *context,
    UmiProcessResult *result);
#define UmiProcessExecuteWithLifetime FakeExecute
#include "../../src/repository/inspection/git_capture.c"
#undef UmiProcessExecuteWithLifetime
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); failed = 1; goto cleanup; \
} } while (0)

/* The fake runner can divide a binary protocol record at any byte boundary.
 * This checks the capture contract without launching processes or using a repo. */
static UmiStatus FakeExecute(const UmiProcessRequest *request, UmiProcessLifetime lifetime,
    UmiProcessResultObserver observer, UmiProcessOutputObserver raw, void *context,
    UmiProcessResult *result)
{
    ++calls;
    validRequest = request != NULL && request->argument_count == 1U &&
        strcmp(request->program, "selected-git") == 0 &&
        strcmp(request->working_directory, "selected-root") == 0 &&
        request->timeout_ms == 1234U && request->capture_stdout && request->capture_stderr &&
        request->window_mode == UMI_PROCESS_WINDOW_HIDDEN &&
        lifetime == UMI_PROCESS_LIFETIME_TREE && observer == NULL && raw != NULL;
    result->launched = strcmp(scenario, "launch") != 0;
    result->exit_code = 0;
    if (strcmp(scenario, "empty") != 0 && result->launched) {
        const char bytes[] = {'a', '\0', 'b', '\n', 'c'};
        if (strcmp(scenario, "bytewise") == 0) {
            for (size_t i = 0U; i < sizeof bytes; ++i) raw(bytes + i, 1U, context);
        } else raw(bytes, sizeof bytes, context);
        raw(NULL, 0U, context);
    }
    if (strcmp(scenario, "nonzero") == 0) { result->exit_code = 1; return UMI_STATUS_INTERNAL_ERROR; }
    if (strcmp(scenario, "exit_only") == 0) result->exit_code = 7;
    if (strcmp(scenario, "timeout") == 0) result->timed_out = 1;
    if (strcmp(scenario, "cancel") == 0) result->cancelled = 1;
    if (strcmp(scenario, "termination") == 0) result->termination_requested = 1;
    if (strcmp(scenario, "tail") == 0) result->output_truncated = 1;
    if (strcmp(scenario, "launch") == 0) { result->exit_code = -1; return UMI_STATUS_IO_ERROR; }
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    int failed = 0;
    UmiRepositoryGitCaptureResult result = {0};
    CHECK(argc == 2);
    scenario = argv[1];
    UmiRepositoryInspectionContext context = {"selected-git", "selected-root", 1234U};
    const char *arguments[] = {"status"};
    if (strcmp(scenario, "invalid") == 0) {
        CHECK(UmiRepositoryGitCapture(NULL, arguments, 1U, 8U, NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(result.bytes == NULL && result.exit_code == -1);
        CHECK(UmiRepositoryGitCapture(&context, arguments, 1U, 0U, NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiRepositoryGitCapture(&context, arguments, 1U,
            UMI_REPOSITORY_GIT_CAPTURE_LIMIT + 1U, NULL, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiRepositoryGitCapture(&context, arguments, UMI_PROCESS_MAX_ARGUMENTS + 1U,
            8U, NULL, &result) == UMI_STATUS_CAPACITY_EXCEEDED);
        const char *invalid[] = {NULL};
        CHECK(UmiRepositoryGitCapture(&context, invalid, 1U, 8U, NULL, &result) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiRepositoryGitCapture(&context, arguments, 1U, 8U, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(calls == 0U);
        goto cleanup;
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(scenario, "nonzero") == 0) expected = UMI_STATUS_INTERNAL_ERROR;
    if (strcmp(scenario, "exit_only") == 0 || strcmp(scenario, "launch") == 0) expected = UMI_STATUS_IO_ERROR;
    if (strcmp(scenario, "timeout") == 0) expected = UMI_STATUS_TIMEOUT;
    if (strcmp(scenario, "cancel") == 0 || strcmp(scenario, "termination") == 0) expected = UMI_STATUS_CANCELLED;
    if (strcmp(scenario, "overflow") == 0) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    size_t limit = strcmp(scenario, "overflow") == 0 ? 3U :
        strcmp(scenario, "exact") == 0 ? 5U : 8U;
    CHECK(UmiRepositoryGitCapture(&context, arguments, 1U, limit, NULL, &result) == expected);
    CHECK(result.status == expected && calls == 1U && validRequest);
    size_t length = strcmp(scenario, "empty") == 0 || strcmp(scenario, "launch") == 0 ? 0U :
        strcmp(scenario, "overflow") == 0 ? 3U : 5U;
    CHECK(result.bytes != NULL && result.length == length && result.bytes[length] == 0U);
    if (length != 0U) CHECK(memcmp(result.bytes, "a\0b\nc", length) == 0);
    CHECK(result.output_truncated == (strcmp(scenario, "overflow") == 0));
cleanup:
    UmiRepositoryGitCaptureRelease(&result);
    UmiRepositoryGitCaptureRelease(&result);
    UmiRepositoryGitCaptureRelease(NULL);
    return failed;
}
