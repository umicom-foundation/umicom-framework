/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/ctest_adapter.c
 *
 * PURPOSE:
 *   Use CTest list and run commands while exposing Framework-owned test records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

#include "umicom/testing/ctest_adapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wchar.h>
#else
#include <unistd.h>
#endif

#include "umicom/platform/process.h"
#include "umicom/testing/discovery.h"

/* Provide the ctest discover operation used by this module and its client applications. */
UmiStatus umi_ctest_discover(const char *build_directory,
                             UmiTestSuite *suite,
                             size_t *out_discovered)
{
    return UmiCtestDiscoverConfigured(build_directory, NULL, suite,
        out_discovered, NULL, 0U);
}

/* Preserve process failures before parsing. In particular, a zero exit code
 * cannot turn truncated capture into a trustworthy complete catalogue. */
UmiStatus UmiCtestDiscoverConfigured(const char *buildDirectory,
    const UmiCtestDiscoveryOptions *options, UmiTestSuite *suite,
    size_t *outDiscovered, char *outDiagnostics, size_t diagnosticsCapacity)
{
    if (outDiscovered != NULL) *outDiscovered = 0U;
    if (outDiagnostics != NULL && diagnosticsCapacity != 0U)
        outDiagnostics[0] = '\0';
    if (buildDirectory == NULL || buildDirectory[0] == '\0' || suite == NULL ||
        (outDiagnostics == NULL && diagnosticsCapacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *message = "Complete CTest catalogue imported.";
    UmiStatus status = UMI_STATUS_OK;
    UmiProcessResult result;
    memset(&result, 0, sizeof result);
    result.exit_code = -1;
    const char *arguments[5] = {"--test-dir", buildDirectory, "-N", NULL, NULL};
    UmiProcessRequest request = {0};
    request.program = "ctest";
    request.arguments = arguments;
    request.argument_count = 3U;
    request.capture_stdout = 1;
    request.capture_stderr = 1;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    request.timeout_ms = options != NULL && options->timeout_ms != 0U
        ? options->timeout_ms : 30000U;
    request.cancellation = options != NULL ? options->cancellation : NULL;
    if (options != NULL && options->configuration != NULL &&
        options->configuration[0] != '\0') {
        arguments[3] = "-C"; arguments[4] = options->configuration;
        request.argument_count = 5U;
    }
    if (strlen(buildDirectory) >= UMI_TEST_COMMAND_CAPACITY) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        message = "CTest build directory exceeds legacy record capacity.";
    } else if (umi_cancellation_token_is_requested(request.cancellation)) {
        status = UMI_STATUS_CANCELLED;
        message = "CTest discovery cancelled before launch; catalogue unchanged.";
    } else {
        /* CTest versions can report an empty catalogue for an unconfigured
         * directory. Distinguish that from a configured project with zero tests
         * before allowing Studio to replace its working catalogue. */
        char testFile[UMI_TEST_COMMAND_CAPACITY + 32U];
        int written = snprintf(testFile, sizeof testFile, "%s/CTestTestfile.cmake",
            buildDirectory);
        FILE *configured = written > 0 && (size_t)written < sizeof testFile
            ? fopen(testFile, "rb") : NULL;
        int processAttempted = 0;
        if (configured == NULL) {
            status = UMI_STATUS_NOT_FOUND;
            message = "No readable CTestTestfile.cmake in the selected build root. Configure tests first.";
        } else if (fclose(configured) != 0) {
            status = UMI_STATUS_IO_ERROR;
            message = "Could not close CTest configuration probe; catalogue unchanged.";
        } else {
            processAttempted = 1;
            status = umi_process_execute(&request, &result);
        }

        if (result.cancelled || status == UMI_STATUS_CANCELLED) {
            status = UMI_STATUS_CANCELLED;
            message = "CTest discovery cancelled; catalogue unchanged.";
        } else if (result.timed_out || status == UMI_STATUS_TIMEOUT) {
            status = UMI_STATUS_TIMEOUT;
            message = "CTest discovery timed out; catalogue unchanged.";
        } else if (status != UMI_STATUS_OK) {
            if (processAttempted)
                message = "CTest discovery process failed; catalogue unchanged.";
        } else if (!result.launched || result.exit_code != 0) {
            status = UMI_STATUS_UNAVAILABLE;
            message = "CTest did not launch successfully or exited with an error.";
        } else if (result.output_truncated) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            message = "CTest output exceeds capture capacity; incomplete discovery was rejected.";
        } else if (memchr(result.output, '\0', sizeof result.output) == NULL) {
            status = UMI_STATUS_PARSE_ERROR;
            message = "CTest process output is not terminated; catalogue unchanged.";
        } else {
            status = UmiTestDiscoveryParseCtestComplete(result.output,
                strlen(result.output), buildDirectory, suite, outDiscovered);
            if (status == UMI_STATUS_CAPACITY_EXCEEDED)
                message = "CTest discovery exceeds legacy suite or identity capacity; catalogue unchanged.";
            else if (status == UMI_STATUS_ALREADY_EXISTS)
                message = "Duplicate CTest identity; catalogue unchanged. Refresh into a new suite.";
            else if (status == UMI_STATUS_UNAVAILABLE)
                message = "Ambiguous disabled marker in human listing; use JSON metadata discovery.";
            else if (status != UMI_STATUS_OK)
                message = "Incomplete or invalid CTest listing; catalogue unchanged.";
        }
    }
    if (outDiagnostics != NULL && diagnosticsCapacity != 0U)
        (void)snprintf(outDiagnostics, diagnosticsCapacity, "%s", message);
    return status;
}


#include "ctest_report.inc"
#include "ctest_report_files.inc"

/* Retain the most recent diagnostic bytes, where compilers and CTest normally
 * print the cause of a failure. Truncation is visible rather than silent. */
static void CtestCopyOutput(UmiTestResult *result, const UmiProcessResult *process)
{
    const char *notice = "[Output truncated; showing latest captured bytes.]\n";
    size_t length = strlen(process->output);
    size_t capacity = sizeof result->output;
    if (process->output_truncated || length >= capacity) {
        size_t prefix = strlen(notice);
        size_t keep = capacity - prefix - 1U;
        if (keep > length) keep = length;
        memcpy(result->output, notice, prefix);
        memcpy(result->output + prefix, process->output + length - keep, keep);
        result->output[prefix + keep] = '\0';
    } else memcpy(result->output, process->output, length + 1U);
}

static void CtestDiagnostic(UmiTestResult *result, const char *message)
{
    char line[384];
    int written = snprintf(line, sizeof line, "\n[Umicom CTest] %s\n", message);
    if (written < 0) return;
    size_t length = (size_t)written;
    if (length >= sizeof line) length = sizeof line - 1U;
    size_t used = strlen(result->output);
    if (length >= sizeof result->output - used) {
        const char *notice = "[Output truncated; showing latest captured bytes.]\n";
        size_t prefix = strlen(notice);
        size_t keep = sizeof result->output - prefix - length - 1U;
        /* Preserve the notice when adding a final diagnostic to a full tail.
         * Simply dropping the oldest bytes also dropped the truncation flag. */
        memmove(result->output + prefix, result->output + used - keep, keep);
        memcpy(result->output, notice, prefix);
        used = prefix + keep;
    }
    memcpy(result->output + used, line, length + 1U);
}

static UmiStatus CtestRunConfigured(const char *buildDirectory,
    const char *testName, const UmiCtestRunOptions *options,
    UmiTestResult *outResult)
{
    if (outResult == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Initialise even on rejected requests: Studio must never inspect garbage
     * as a state, duration, exit code or captured output after an early error. */
    memset(outResult, 0, sizeof *outResult);
    outResult->state = UMI_TEST_STATE_NOT_RUN;
    outResult->status = UMI_STATUS_INVALID_ARGUMENT;
    outResult->exit_code = -1;
    char pattern[UMI_CTEST_EXACT_PATTERN_CAPACITY];
    UmiStatus status = UmiCtestEscapeName(testName, pattern, sizeof pattern);
    if (status != UMI_STATUS_OK) { outResult->status = status; return status; }
    if (buildDirectory == NULL || buildDirectory[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *testId = options != NULL && options->test_id != NULL
        ? options->test_id : testName;
    if (testId[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(testId) >= sizeof outResult->test_id) {
        outResult->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return outResult->status;
    }
    memcpy(outResult->test_id, testId, strlen(testId) + 1U);
    memcpy(outResult->name, testName, strlen(testName) + 1U);
    if (options != NULL && umi_cancellation_token_is_requested(options->cancellation)) {
        outResult->state = UMI_TEST_STATE_CANCELLED;
        outResult->status = UMI_STATUS_CANCELLED;
        CtestDiagnostic(outResult, "Cancelled before launching CTest.");
        return outResult->status;
    }
    UmiCtestReportFiles files;
    char *xml = NULL; size_t xmlLength = 0U;
    status = CtestReportCreate(&files);
    if (status != UMI_STATUS_OK) {
        CtestDiagnostic(outResult, "Cannot create a fresh private CTest report directory.");
        goto cleanup;
    }
    const char *arguments[10]; size_t count = 0U;
    arguments[count++] = "--test-dir"; arguments[count++] = buildDirectory;
    arguments[count++] = "-R"; arguments[count++] = pattern;
    arguments[count++] = "--output-on-failure";
    arguments[count++] = "--no-tests=error";
    arguments[count++] = "--output-junit"; arguments[count++] = files.path;
    if (options != NULL && options->configuration != NULL &&
        options->configuration[0] != '\0') {
        arguments[count++] = "-C"; arguments[count++] = options->configuration;
    }
    UmiProcessRequest request;
    UmiProcessResult process;
    memset(&request, 0, sizeof request);
    memset(&process, 0, sizeof process); process.exit_code = -1;
    request.program = "ctest";
    request.arguments = arguments; request.argument_count = count;
    request.capture_stdout = 1; request.capture_stderr = 1;
    request.window_mode = UMI_PROCESS_WINDOW_HIDDEN;
    if (options != NULL) {
        request.timeout_ms = options->timeout_ms;
        request.cancellation = options->cancellation;
    }
    /* No shell parser, command-string interpolation or global chdir is used.
     * CTest itself owns test properties, fixtures and WORKING_DIRECTORY. */
    UmiStatus processStatus = umi_process_execute(&request, &process);
    CtestCopyOutput(outResult, &process);
    outResult->exit_code = process.exit_code;
    outResult->duration_ms = process.duration_ms;
    if (process.cancelled || processStatus == UMI_STATUS_CANCELLED) {
        outResult->state = UMI_TEST_STATE_CANCELLED; status = UMI_STATUS_CANCELLED;
        CtestDiagnostic(outResult, "CTest was cancelled; no pass is inferred from a partial report.");
        goto cleanup;
    }
    if (process.timed_out || processStatus == UMI_STATUS_TIMEOUT) {
        outResult->state = UMI_TEST_STATE_TIMED_OUT; status = UMI_STATUS_TIMEOUT;
        CtestDiagnostic(outResult, "The complete CTest invocation exceeded its time limit.");
        goto cleanup;
    }
    if (!process.launched) {
        status = processStatus != UMI_STATUS_OK ? processStatus : UMI_STATUS_UNAVAILABLE;
        CtestDiagnostic(outResult, "CTest did not launch; the selected test was not run.");
        goto cleanup;
    }
    status = CtestReportRead(&files, &xml, &xmlLength);
    if (status != UMI_STATUS_OK) {
        CtestDiagnostic(outResult, "CTest did not produce a readable bounded JUnit report (CTest 3.21+ required).");
        goto cleanup;
    }
    UmiCtestReportSnapshot report;
    status = UmiCtestParseReport(xml, xmlLength, testName, &report);
    if (status != UMI_STATUS_OK) {
        CtestDiagnostic(outResult, status == UMI_STATUS_NOT_FOUND
            ? "The report contains no exact result for the selected test. Refresh discovery and check the build root."
            : "The CTest report is malformed, inconsistent, duplicated or outside the supported limits.");
        goto cleanup;
    }
    outResult->state = report.state;
    outResult->duration_ms = report.duration_ms;
    if (report.message[0] != '\0') CtestDiagnostic(outResult, report.message);
    if (report.state == UMI_TEST_STATE_TIMED_OUT) status = UMI_STATUS_TIMEOUT;
    else if (report.state == UMI_TEST_STATE_FAILED) status = UMI_STATUS_INTERNAL_ERROR;
    else if (report.state == UMI_TEST_STATE_NOT_RUN) status = UMI_STATUS_INVALID_STATE;
    else if (processStatus != UMI_STATUS_OK || process.exit_code != 0) {
        status = processStatus != UMI_STATUS_OK ? processStatus : UMI_STATUS_INTERNAL_ERROR;
        /* A passing selected body is insufficient when a fixture, cleanup or
         * CTest itself fails. Preserve the evidence, but fail the whole run. */
        if (outResult->state == UMI_TEST_STATE_PASSED)
            outResult->state = UMI_TEST_STATE_FAILED;
        CtestDiagnostic(outResult, "CTest reported a run-level failure; inspect fixture and cleanup output.");
    } else status = UMI_STATUS_OK;
cleanup:
    free(xml);
    UmiStatus cleanupStatus = CtestReportCleanup(&files);
    if (cleanupStatus != UMI_STATUS_OK) {
        CtestDiagnostic(outResult, "Temporary report cleanup failed; unexpected files were not removed recursively.");
        if (status == UMI_STATUS_OK) status = cleanupStatus;
        if (outResult->state == UMI_TEST_STATE_PASSED)
            outResult->state = UMI_TEST_STATE_FAILED;
    }
    outResult->status = status;
    return status;
}

/* Stage the result before publishing it, so a caller may reuse result.name or
 * result.test_id as an input without the initial reset destroying that input. */
UmiStatus UmiCtestRunConfigured(const char *buildDirectory,
    const char *testName, const UmiCtestRunOptions *options,
    UmiTestResult *outResult)
{
    if (outResult == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTestResult result;
    UmiStatus status = CtestRunConfigured(buildDirectory, testName, options, &result);
    *outResult = result;
    return status;
}

/*
 * Perform ctest through the module contract so client applications do not duplicate its
 * policy.
 */
UmiStatus umi_ctest_run(const char *build_directory,
                        const char *test_name,
                        UmiTestResult *out_result)
{
    /* Migration: the previous body formatted
     *   --test-dir \"%s\" -R \"^%s$\" --output-on-failure
     * into UmiTestCase.arguments, then called umi_test_runner_run_case(). That
     * made metacharacters active, could truncate the command, and equated CTest
     * exit zero with a passing testcase. The generic runner and its public API
     * remain available. CTest-specific execution now has one Framework owner:
     * UmiCtestRunConfigured() above, with literal argv and a fresh result report.
     */
    return UmiCtestRunConfigured(build_directory, test_name, NULL, out_result);
}

/* Preserve catalogue ownership and ordering while sharing the exact same
 * execution policy as a selected test. Applications no longer need to fall
 * back to a command-string runner to obtain an aggregate CTest summary. */
UmiStatus UmiCtestRunSuite(const char *buildDirectory, const UmiTestSuite *suite,
    const UmiCtestRunOptions *options, UmiTestResult *results,
    size_t resultCapacity, UmiTestRunSummary *outSummary)
{
    if (outSummary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(outSummary, 0, sizeof *outSummary);
    if (buildDirectory == NULL || buildDirectory[0] == '\0' || suite == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t count = umi_test_suite_count(suite);
    if (count == 0U) return UMI_STATUS_NOT_FOUND;
    if (results == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (count > resultCapacity || count > SIZE_MAX / sizeof *results)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus firstFailure = UMI_STATUS_OK;
    for (size_t index = 0U; index < count; ++index) {
        const UmiTestCase *testCase = umi_test_suite_at(suite, index);
        UmiTestResult *result = &results[index];
        memset(result, 0, sizeof *result);
        result->state = UMI_TEST_STATE_NOT_RUN; result->exit_code = -1;
        UmiStatus status;
        if (testCase == NULL ||
            memchr(testCase->test_id, '\0', sizeof testCase->test_id) == NULL ||
            memchr(testCase->name, '\0', sizeof testCase->name) == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            CtestDiagnostic(result, "A catalogue record has invalid bounded identity storage.");
        } else if (!testCase->enabled) {
            memcpy(result->test_id, testCase->test_id, sizeof result->test_id);
            memcpy(result->name, testCase->name, sizeof result->name);
            result->state = UMI_TEST_STATE_SKIPPED; status = UMI_STATUS_OK;
            CtestDiagnostic(result, "The catalogue disabled this test; no process was launched.");
        } else {
            UmiCtestRunOptions runOptions = {0};
            if (options != NULL) runOptions = *options;
            runOptions.test_id = testCase->test_id;
            if (testCase->timeout_ms != 0U) runOptions.timeout_ms = testCase->timeout_ms;
            status = UmiCtestRunConfigured(buildDirectory, testCase->name, &runOptions, result);
        }
        result->status = status;
        ++outSummary->total;
        if (result->duration_ms > UINT64_MAX - outSummary->duration_ms) {
            outSummary->duration_ms = UINT64_MAX;
            if (firstFailure == UMI_STATUS_OK) firstFailure = UMI_STATUS_CAPACITY_EXCEEDED;
        } else outSummary->duration_ms += result->duration_ms;
        switch (result->state) {
            case UMI_TEST_STATE_PASSED:
                if (UmiCtestResultRequirePassed(result) == UMI_STATUS_OK) ++outSummary->passed;
                else ++outSummary->failed;
                break;
            case UMI_TEST_STATE_SKIPPED: ++outSummary->skipped; break;
            case UMI_TEST_STATE_CANCELLED: ++outSummary->cancelled; break;
            case UMI_TEST_STATE_TIMED_OUT: ++outSummary->timed_out; break;
            default: ++outSummary->failed; break;
        }
        if (status != UMI_STATUS_OK && firstFailure == UMI_STATUS_OK)
            firstFailure = status;
    }
    return firstFailure;
}

/* Queue-backed runs reuse this translation unit's verified CTest adapter. */
#include "ctest_job.inc"
