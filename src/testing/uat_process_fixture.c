/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/testing/uat_process_fixture.c
 *
 * PURPOSE:
 *   Map named UAT steps to trusted native tool arguments and verify stdout.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/uat_process_fixture.h"
#include "umicom/platform/path.h"
#include "umicom/platform/process.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct OutputComparison {
    const unsigned char *expected;
    size_t remaining;
    bool differs;
} OutputComparison;

/* Copy portable expected exits into the admitted synchronous invocation. The
 * public probe and fixture layouts remain available to existing consumers. */
typedef struct ProcessFixtureExecution {
    UmiUatProcessFixture fixture;
    int expectedExitCodes[UMI_UAT_MAX_STEPS];
    bool reportsExitCode;
    const UmiUatProcessExpectedOutcome *outcomes;
} ProcessFixtureExecution;

static void CompareOutput(const char *bytes, size_t length, void *context)
{
    OutputComparison *comparison = context;
    if (comparison->differs || length == 0U) return;
    /* Compare the byte stream before the runner's diagnostic tail rotates.
     * Keeping a cursor avoids retaining raw output in the acceptance report. */
    if (length > comparison->remaining || memcmp(bytes, comparison->expected, length) != 0) {
        comparison->differs = true;
        return;
    }
    comparison->expected += length;
    comparison->remaining -= length;
}

static void CompareChannels(UmiProcessOutputChannel channel, const char *bytes, size_t length, void *context)
{
    /* The Platform runner identifies each native pipe before diagnostic
     * aggregation. Independent cursors preserve exact bytes and order. */
    OutputComparison *comparisons = context;
    CompareOutput(bytes, length, &comparisons[channel == UMI_PROCESS_OUTPUT_STDOUT ? 0U : 1U]);
}

static const UmiUatProcessProbe *FindProbe(const UmiUatProcessFixture *fixture, const char *id)
{
    for (size_t i = 0U; i < fixture->probeCount; ++i)
        if (strcmp(fixture->probes[i].testId, id) == 0) return &fixture->probes[i];
    return NULL;
}

static UmiStatus CheckProbes(const UmiUatProcessFixture *fixture, uint32_t *supported)
{
    if (fixture == NULL || fixture->probes == NULL || fixture->probeCount == 0U ||
        fixture->probeCount > UMI_UAT_MAX_STEPS) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < fixture->probeCount; ++i) {
        const UmiUatProcessProbe *probe = &fixture->probes[i];
        const uint32_t operation = (uint32_t)probe->operation;
        if (probe->testId == NULL || probe->testId[0] == '\0' || strlen(probe->testId) >= UMI_TEST_ID_CAPACITY ||
            !umi_path_is_absolute(probe->program) || !umi_path_is_absolute(probe->workingDirectory) ||
            probe->timeoutLimitMs == 0U || probe->argumentCount > UMI_PROCESS_MAX_ARGUMENTS ||
            (probe->argumentCount != 0U && probe->arguments == NULL) ||
            (probe->expectedStdoutLength != 0U && probe->expectedStdout == NULL) ||
            operation == 0U || (operation & (operation - 1U)) != 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (probe->expectedStdoutLength > UMI_UAT_PROCESS_EXPECTED_LIMIT) return UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t argument = 0U; argument < probe->argumentCount; ++argument)
            if (probe->arguments[argument] == NULL) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t earlier = 0U; earlier < i; ++earlier)
            if (strcmp(fixture->probes[earlier].testId, probe->testId) == 0) return UMI_STATUS_ALREADY_EXISTS;
        *supported |= operation;
    }
    return UMI_STATUS_OK;
}

static UmiStatus RunProbe(void *context, const UmiUatTarget *target, UmiUatOperation operation,
    const UmiTestCase *test, UmiCancellationToken *cancellation, UmiTestResult *result)
{
    (void)target;
    ProcessFixtureExecution *execution = context;
    const UmiUatProcessProbe *probe = FindProbe(&execution->fixture, test->test_id);
    if (probe == NULL || operation != probe->operation) return UMI_STATUS_INVALID_STATE;
    const size_t probeIndex = (size_t)(probe - execution->fixture.probes);
    int expectedExit = execution->expectedExitCodes[probeIndex];
    const UmiProcessRequest request = {.program = probe->program, .arguments = probe->arguments,
        .argument_count = probe->argumentCount, .working_directory = probe->workingDirectory,
        .capture_stdout = 1, .timeout_ms = test->timeout_ms, .cancellation = cancellation,
        .window_mode = UMI_PROCESS_WINDOW_HIDDEN};
    OutputComparison comparison = {probe->expectedStdout, probe->expectedStdoutLength, false};
    OutputComparison channels[2] = {comparison, {0}};
    UmiProcessResult child = {0};
    UmiStatus status;
    if (execution->outcomes != NULL) {
        /* Use a single process so both observations attest the same outcome. */
        const UmiUatProcessExpectedOutcome *outcome = &execution->outcomes[probeIndex];
        channels[1] = (OutputComparison){outcome->stderrBytes, outcome->stderrLength, false};
        status = UmiProcessExecuteChannels(&request, UMI_PROCESS_LIFETIME_TREE, CompareChannels, channels, &child);
        comparison = channels[0];
    } else {
        /* The original probes capture only stdout and retain their established
         * report messages and native stderr behavior. */
        status = UmiProcessExecuteWithLifetime(&request, UMI_PROCESS_LIFETIME_TREE,
            NULL, CompareOutput, &comparison, &child);
    }
    /* The Platform runner treats a completed nonzero exit as INTERNAL_ERROR.
     * Only a completed, exactly expected normal exit can satisfy a rejection
     * assertion. Keep the former success-only condition for owner review. */
#if 0
    if (status == UMI_STATUS_OK && (!child.launched || child.exit_code != 0)) status = UMI_STATUS_INVALID_STATE;
#endif
    bool completed = child.launched && !child.cancelled && !child.timed_out && !child.termination_requested &&
        (status == UMI_STATUS_OK || status == UMI_STATUS_INTERNAL_ERROR);
    if (expectedExit != 0 && completed && status == UMI_STATUS_INTERNAL_ERROR && child.exit_code == expectedExit)
        status = UMI_STATUS_OK;
    if (status == UMI_STATUS_OK && (!child.launched || child.exit_code != expectedExit))
        status = UMI_STATUS_INVALID_STATE;
    if (execution->reportsExitCode && completed && child.exit_code != expectedExit)
        (void)snprintf(result->output, sizeof result->output, "Process exit was %d. Expected %d.",
            child.exit_code, expectedExit);
    if (status == UMI_STATUS_OK && (comparison.differs || comparison.remaining != 0U)) {
        status = UMI_STATUS_INVALID_STATE;
        memcpy(result->output, "Process stdout differed from the expected bytes.",
            sizeof "Process stdout differed from the expected bytes.");
    }
    if (status == UMI_STATUS_OK && (channels[1].differs || channels[1].remaining != 0U)) {
        status = UMI_STATUS_INVALID_STATE;
        memcpy(result->output, "Process stderr differed from the expected bytes.",
            sizeof "Process stderr differed from the expected bytes.");
    }
    /* The shared workflow stores the assertion exit code as zero on success.
     * The optional message records the exact child code without saving stdout. */
    if (status == UMI_STATUS_OK && execution->outcomes != NULL)
        (void)snprintf(result->output, sizeof result->output, "Process exit %d, stdout and stderr matched.", child.exit_code);
    else if (status == UMI_STATUS_OK && execution->reportsExitCode)
        (void)snprintf(result->output, sizeof result->output, "Process exit %d and stdout matched.", child.exit_code);
    else if (status == UMI_STATUS_OK)
        memcpy(result->output, "Process exited successfully and stdout matched.",
            sizeof "Process exited successfully and stdout matched.");
    /* Preserve the original raw exit handoff for review. The shared workflow
     * expects zero for a successful assertion, including an expected rejection. */
#if 0
    umi_test_result_finish(result, status, child.launched ? child.exit_code : -1, child.duration_ms);
#endif
    umi_test_result_finish(result, status, status == UMI_STATUS_OK ? 0 : child.launched ? child.exit_code : -1,
        child.duration_ms);
    return status;
}

static UmiStatus RunProcessFixture(const UmiUatTarget *target, uint32_t allowedOperations,
    const UmiUatProcessFixture *fixture, const int *expectedExitCodes, size_t exitCodeCount,
    const UmiUatProcessExpectedOutcome *outcomes, size_t outcomeCount,
    const UmiUatStep *steps, size_t count,
    UmiCancellationToken *cancellation, UmiCancellationToken *cleanupCancellation,
    UmiUatReport **outReport)
{
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport = NULL;
    if (target == NULL || steps == NULL || count == 0U || count > UMI_UAT_MAX_STEPS)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiUatDriver driver = {0};
    UmiStatus status = CheckProbes(fixture, &driver.supportedOperations);
    if (status != UMI_STATUS_OK) return status;
    ProcessFixtureExecution execution = {.fixture = *fixture,
        .reportsExitCode = expectedExitCodes != NULL || outcomes != NULL, .outcomes = outcomes};
    if (outcomes != NULL) {
        /* Validate all outcome spans before running setup or any process. */
        if (outcomeCount != fixture->probeCount) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t i = 0U; i < outcomeCount; ++i) {
            const UmiUatProcessExpectedOutcome *outcome = &outcomes[i];
            if (outcome->exitCode < 0 || outcome->exitCode > 125 ||
                (outcome->stderrLength != 0U && outcome->stderrBytes == NULL))
                return UMI_STATUS_INVALID_ARGUMENT;
            if (outcome->stderrLength > UMI_UAT_PROCESS_EXPECTED_LIMIT)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            execution.expectedExitCodes[i] = outcome->exitCode;
        }
    } else if (execution.reportsExitCode) {
        if (exitCodeCount != fixture->probeCount) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t i = 0U; i < exitCodeCount; ++i) {
            /* Platform maps POSIX signals into the high exit-code range.
             * Keep expected results inside portable ordinary process exits. */
            if (expectedExitCodes[i] < 0 || expectedExitCodes[i] > 125)
                return UMI_STATUS_INVALID_ARGUMENT;
            execution.expectedExitCodes[i] = expectedExitCodes[i];
        }
    }
    /* A test plan names fixture records. The legacy process fields never
     * authorize a path or argument. Stage only trusted command identity for
     * the existing case validator and leave the arguments at their owner. */
    UmiUatStep *prepared = malloc(count * sizeof *prepared);
    if (prepared == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiUatOperation operations[UMI_UAT_MAX_STEPS];
    for (size_t i = 0U; i < count && status == UMI_STATUS_OK; ++i) {
        const UmiTestCase *test = &steps[i].test;
        if (test->test_id[0] == '\0' || memchr(test->test_id, '\0', sizeof test->test_id) == NULL) {
            status = UMI_STATUS_INVALID_ARGUMENT;
            break;
        }
        const UmiUatProcessProbe *probe = FindProbe(fixture, test->test_id);
        if (probe == NULL) {
            status = UMI_STATUS_NOT_FOUND;
            break;
        }
        if (test->program[0] != '\0' || test->argument_text[0] != '\0' || test->working_directory[0] != '\0' ||
            test->timeout_ms > probe->timeoutLimitMs) {
            status = UMI_STATUS_PERMISSION_DENIED;
            break;
        }
        prepared[i] = steps[i];
        memcpy(prepared[i].test.program, "native-fixture", sizeof "native-fixture");
        operations[i] = probe->operation;
    }
    if (status == UMI_STATUS_OK) {
        driver.binding = execution.fixture.binding;
        driver.execute = RunProbe;
        driver.context = &execution;
        status = UmiUatRunTarget(target, allowedOperations, &driver, prepared, operations, count,
            cancellation, cleanupCancellation, outReport);
    }
    free(prepared);
    return status;
}

/* Preserve the existing default of exit zero for every approved probe. */
UmiStatus UmiUatRunProcessFixture(const UmiUatTarget *target, uint32_t allowedOperations,
    const UmiUatProcessFixture *fixture, const UmiUatStep *steps, size_t count,
    UmiCancellationToken *cancellation, UmiCancellationToken *cleanupCancellation, UmiUatReport **outReport)
{
    return RunProcessFixture(target, allowedOperations, fixture, NULL, 0U, NULL, 0U, steps, count,
        cancellation, cleanupCancellation, outReport);
}

/* An explicit table is required for the extension. The shared admission path
 * checks its length and every entry before constructing a process plan. */
UmiStatus UmiUatRunProcessFixtureWithExitCodes(const UmiUatTarget *target, uint32_t allowedOperations,
    const UmiUatProcessFixture *fixture, const int *expectedExitCodes, size_t exitCodeCount,
    const UmiUatStep *steps, size_t count, UmiCancellationToken *cancellation,
    UmiCancellationToken *cleanupCancellation, UmiUatReport **outReport)
{
    if (expectedExitCodes == NULL) {
        if (outReport != NULL) *outReport = NULL;
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return RunProcessFixture(target, allowedOperations, fixture, expectedExitCodes, exitCodeCount, NULL, 0U,
        steps, count, cancellation, cleanupCancellation, outReport);
}

UmiStatus UmiUatRunProcessFixtureWithOutcomes(const UmiUatTarget *target, uint32_t allowedOperations,
    const UmiUatProcessFixture *fixture, const UmiUatProcessExpectedOutcome *outcomes, size_t outcomeCount,
    const UmiUatStep *steps, size_t count, UmiCancellationToken *cancellation,
    UmiCancellationToken *cleanupCancellation, UmiUatReport **outReport)
{
    /* Require an explicit comparison for each approved process. */
    if (outcomes == NULL) {
        if (outReport != NULL) *outReport = NULL;
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return RunProcessFixture(target, allowedOperations, fixture, NULL, 0U, outcomes, outcomeCount,
        steps, count, cancellation, cleanupCancellation, outReport);
}
