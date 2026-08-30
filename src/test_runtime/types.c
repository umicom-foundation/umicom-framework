/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_runtime/types.c
 *
 * PURPOSE:
 *   Implement stable names and bounded text helpers for the test runtime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/test_runtime/types.h"
#include <string.h>

static size_t bounded_text_length(const char *text, size_t capacity)
{
    size_t length = 0U;
    if (text == NULL) return capacity;
    while (length < capacity && text[length] != '\0') ++length;
    return length;
}

const char *umi_test_runtime_state_text(UmiTestRuntimeState state)
{
    switch (state) {
    case UMI_TEST_RUNTIME_STATE_CREATED: return "created";
    case UMI_TEST_RUNTIME_STATE_READY: return "ready";
    case UMI_TEST_RUNTIME_STATE_STARTING: return "starting";
    case UMI_TEST_RUNTIME_STATE_RUNNING: return "running";
    case UMI_TEST_RUNTIME_STATE_PASSED: return "passed";
    case UMI_TEST_RUNTIME_STATE_FAILED: return "failed";
    case UMI_TEST_RUNTIME_STATE_NOT_RUN: return "not-run";
    case UMI_TEST_RUNTIME_STATE_TIMED_OUT: return "timed-out";
    case UMI_TEST_RUNTIME_STATE_CRASHED: return "crashed";
    case UMI_TEST_RUNTIME_STATE_CANCELLED: return "cancelled";
    default: return "unknown";
    }
}

const char *umi_test_runtime_failure_kind_text(UmiTestRuntimeFailureKind kind)
{
    switch (kind) {
    case UMI_TEST_RUNTIME_FAILURE_NONE: return "none";
    case UMI_TEST_RUNTIME_FAILURE_BAD_COMMAND: return "bad-command";
    case UMI_TEST_RUNTIME_FAILURE_MISSING_EXECUTABLE: return "missing-executable";
    case UMI_TEST_RUNTIME_FAILURE_MISSING_RUNTIME_DEPENDENCY: return "missing-runtime-dependency";
    case UMI_TEST_RUNTIME_FAILURE_INVALID_WORKING_DIRECTORY: return "invalid-working-directory";
    case UMI_TEST_RUNTIME_FAILURE_TIMEOUT: return "timeout";
    case UMI_TEST_RUNTIME_FAILURE_ASSERTION: return "assertion";
    case UMI_TEST_RUNTIME_FAILURE_CRASH: return "crash";
    case UMI_TEST_RUNTIME_FAILURE_ACCESS_VIOLATION: return "access-violation";
    case UMI_TEST_RUNTIME_FAILURE_STACK_OVERFLOW: return "stack-overflow";
    case UMI_TEST_RUNTIME_FAILURE_SANITIZER: return "sanitizer";
    case UMI_TEST_RUNTIME_FAILURE_NONZERO_EXIT: return "nonzero-exit";
    case UMI_TEST_RUNTIME_FAILURE_CONFIGURATION: return "configuration";
    case UMI_TEST_RUNTIME_FAILURE_UNKNOWN: return "unknown";
    case UMI_TEST_RUNTIME_FAILURE_FAST_FAIL: return "fast-fail";
    default: return "invalid";
    }
}

const char *umi_test_runtime_profile_kind_text(UmiTestRuntimeProfileKind kind)
{
    switch (kind) {
    case UMI_TEST_RUNTIME_PROFILE_SMOKE: return "smoke";
    case UMI_TEST_RUNTIME_PROFILE_FRAMEWORK_CORE: return "framework-core";
    case UMI_TEST_RUNTIME_PROFILE_LAYOUT: return "layout";
    case UMI_TEST_RUNTIME_PROFILE_DATA: return "data";
    case UMI_TEST_RUNTIME_PROFILE_DESIGNER: return "designer";
    case UMI_TEST_RUNTIME_PROFILE_CONTEXT: return "context";
    case UMI_TEST_RUNTIME_PROFILE_EDITOR: return "editor";
    case UMI_TEST_RUNTIME_PROFILE_STUDIO: return "studio";
    case UMI_TEST_RUNTIME_PROFILE_INTEGRATION: return "integration";
    case UMI_TEST_RUNTIME_PROFILE_FULL_REGRESSION: return "full-regression";
    default: return "unknown";
    }
}

UmiStatus umi_test_runtime_copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = bounded_text_length(source, capacity);
    if (length >= capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    if (length != 0U) memcpy(destination, source, length);
    destination[length] = '\0';
    return UMI_STATUS_OK;
}

bool umi_test_runtime_text_is_valid(const char *text, size_t capacity)
{
    return text != NULL && capacity != 0U && bounded_text_length(text, capacity) < capacity;
}
