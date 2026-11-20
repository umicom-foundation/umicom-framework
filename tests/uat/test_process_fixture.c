/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/uat/test_process_fixture.c
 *
 * PURPOSE:
 *   Qualify named native UAT fixtures with real bounded child processes.
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
#include "umicom/testing/uat_process_fixture.h"
#include "umicom/platform/process.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#include <windows.h>
#else
#include <time.h>
#endif

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        exit(1); \
    } \
} while (0)

static const unsigned char expected[] = {'A', 0U, 'B', '\n'};
static const unsigned char expectedError[] = {'A', 0U, 'C', '\n'};
static const char privateText[] = "private-fixture-sample";

enum { STRESS_BLOCK = 1024, STRESS_COUNT = 160 };

static unsigned char Pattern(size_t offset, UmiProcessOutputChannel channel)
{
    /* The high byte and zero vary with position and stream, so text conversion,
     * reordering or one misplaced chunk cannot pass a matching length check. */
    return (unsigned char)((offset * 19U + offset / 251U +
        (channel == UMI_PROCESS_OUTPUT_STDOUT ? 0U : 43U)) % 256U);
}

static int Child(int argc, char **argv)
{
    const char *mode = argv[2];
#ifdef _WIN32
    CHECK(_setmode(_fileno(stdout), _O_BINARY) != -1);
#endif
    if (strcmp(mode, "noop") == 0) return 0;
    if (strcmp(mode, "witness") == 0) {
        CHECK(argc == 4);
        FILE *file = fopen(argv[3], "wbx");
        CHECK(file != NULL && fclose(file) == 0);
        return 0;
    }
    if (strcmp(mode, "timeout") == 0) {
#ifdef _WIN32
        Sleep(2500U);
#else
        const struct timespec delay = {2, 500000000L};
        CHECK(nanosleep(&delay, NULL) == 0);
#endif
        return 0;
    }
    if (strncmp(mode, "dual-child", 10U) == 0) {
        /* Emit independent binary and terminal outcomes through the actual
         * native handles. Binary mode keeps Windows line endings literal. */
#ifdef _WIN32
        CHECK(_setmode(_fileno(stderr), _O_BINARY) != -1);
#endif
        if (strcmp(mode, "dual-child-raw") == 0) {
            /* Feed both pipes beyond the legacy diagnostic-tail capacity. */
            for (size_t block = 0U; block < STRESS_COUNT; ++block) {
                unsigned char out[STRESS_BLOCK], err[STRESS_BLOCK];
                for (size_t index = 0U; index < sizeof out; ++index) {
                    out[index] = Pattern(block * STRESS_BLOCK + index, UMI_PROCESS_OUTPUT_STDOUT);
                    err[index] = Pattern(block * STRESS_BLOCK + index, UMI_PROCESS_OUTPUT_STDERR);
                }
                CHECK(fwrite(out, 1U, sizeof out, stdout) == sizeof out);
                CHECK(fwrite(err, 1U, sizeof err, stderr) == sizeof err);
            }
            return 0;
        }
        if (strcmp(mode, "dual-child-long") == 0) {
            char block[1024];
            memset(block, 'E', sizeof block);
            for (size_t i = 0U; i < 70U; ++i)
                CHECK(fwrite(block, 1U, sizeof block, stderr) == sizeof block);
        }
        CHECK(fwrite(expected, 1U, sizeof expected, stdout) == sizeof expected);
        CHECK(fflush(stdout) == 0);
        if (strcmp(mode, "dual-child-private") == 0)
            CHECK(fputs(privateText, stderr) != EOF);
        else
            CHECK(fwrite(expectedError, 1U, sizeof expectedError, stderr) == sizeof expectedError);
        return 7;
    }
    if (strcmp(mode, "pass") == 0) {
        CHECK(argc == 4 && strcmp(argv[3], "quoted \"field\" C:\\lesson\\") == 0);
    } else if (strcmp(mode, "exact-tail") == 0) {
        char block[1024];
        memset(block, 'Z', sizeof block);
        for (size_t i = 0U; i < 64U; ++i) CHECK(fwrite(block, 1U, sizeof block, stdout) == sizeof block);
    } else if (strcmp(mode, "mismatch") == 0) {
        return fputs(privateText, stdout) == EOF ? 1 : 0;
    } else if (strcmp(mode, "short") == 0) {
        return fwrite(expected, 1U, sizeof expected - 1U, stdout) == sizeof expected - 1U ? 0 : 1;
    } else if (strcmp(mode, "separate-streams") == 0) {
        CHECK(fputs("fixture messages on stderr\n", stderr) != EOF);
    }
    CHECK(fwrite(expected, 1U, sizeof expected, stdout) == sizeof expected);
    if (strcmp(mode, "prefix") == 0) CHECK(fputc('x', stdout) != EOF);
    return strcmp(mode, "child-error") == 0 ? 7 : 0;
}

static UmiUatStep Step(UmiUatPhase phase, const char *name)
{
    UmiUatStep step = {.phase = phase};
    umi_test_case_init(&step.test, name, name);
    step.test.kind = UMI_TEST_KIND_EXTERNAL;
    step.test.timeout_ms = 5000U;
    return step;
}

static void NoWitness(const char *path)
{
    FILE *file = fopen(path, "rb");
    CHECK(file == NULL && errno == ENOENT);
}

static void VerifyRawChannel(UmiProcessOutputChannel channel, const char *bytes, size_t length, void *context)
{
    CHECK(channel == UMI_PROCESS_OUTPUT_STDOUT || channel == UMI_PROCESS_OUTPUT_STDERR);
    size_t *positions = context;
    size_t *position = &positions[channel == UMI_PROCESS_OUTPUT_STDOUT ? 0U : 1U];
    /* Check each callback against the one position for that native pipe. */
    CHECK(length != 0U && length <= (size_t)STRESS_BLOCK * STRESS_COUNT - *position);
    for (size_t index = 0U; index < length; ++index)
        CHECK((unsigned char)bytes[index] == Pattern((*position)++, channel));
}

static void VerifyCaptureBeyondTail(const char *program, const char *directory)
{
    const char *arguments[] = {"--child", "dual-child-raw"};
    const UmiProcessRequest request = {.program = program, .working_directory = directory,
        .arguments = arguments, .argument_count = 2U, .timeout_ms = 8000U,
        .window_mode = UMI_PROCESS_WINDOW_HIDDEN};
    size_t positions[2] = {0};
    UmiProcessResult result;
    /* This entry enables its own two capture flags. The caller's settings
     * remain untouched and the byte cursors are independent of its tail. */
    CHECK(UmiProcessExecuteChannels(&request, UMI_PROCESS_LIFETIME_TREE,
        VerifyRawChannel, positions, &result) == UMI_STATUS_OK);
    CHECK(result.launched && result.exit_code == 0 && !result.cancelled && !result.timed_out);
    CHECK(positions[0] == (size_t)STRESS_BLOCK * STRESS_COUNT && positions[0] == positions[1]);
    CHECK(request.capture_stdout == 0 && request.capture_stderr == 0);
}

int main(int argc, char **argv)
{
    if (argc >= 3 && strcmp(argv[1], "--child") == 0) return Child(argc, argv);
    CHECK(argc == 4);
    const char *mode = argv[1];
    const char *program = argv[2];
    const char *directory = argv[3];
    if (strcmp(mode, "dual-raw-stress") == 0) {
        VerifyCaptureBeyondTail(program, directory);
        return 0;
    }
    UmiUatTarget target = {UMI_UAT_TARGET_PROTOCOL, UMI_UAT_WEB, "local-ui", "fixture-session"};
    UmiUatStep steps[] = {Step(UMI_UAT_SETUP, "setup"), Step(UMI_UAT_ASSERT, "observe"),
                          Step(UMI_UAT_ASSERT, "after"), Step(UMI_UAT_CLEANUP, "cleanup")};
    const char *noOutput[] = {"--child", "noop"};
    const char *observation[] = {"--child", mode, "quoted \"field\" C:\\lesson\\"};
    UmiUatProcessProbe probes[] = {
        {"setup", UMI_UAT_WORKFLOW, program, directory, noOutput, 2U, 5000U, NULL, 0U},
        {"observe", UMI_UAT_UI_OBSERVE, program, directory, observation, 3U, 5000U, expected, sizeof expected},
        {"after", UMI_UAT_UNIT, program, directory, noOutput, 2U, 5000U, NULL, 0U},
        {"cleanup", UMI_UAT_WORKFLOW, program, directory, noOutput, 2U, 5000U, NULL, 0U}};
    const size_t count = sizeof steps / sizeof steps[0];
    const UmiUatProcessFixture fixture = {target, probes, count};
    /* Each test-controlled code is attached to its fixture entry, never to
     * executable text in the plan. The legacy run still defaults to zero. */
    const bool explicit_exit = strncmp(mode, "exit-", 5U) == 0;
    const bool dual = strncmp(mode, "dual-", 5U) == 0;
    int expected_exits[] = {0, 7, 0, 0};
    UmiUatProcessExpectedOutcome outcomes[] = {{0}, {7, expectedError, sizeof expectedError}, {0}, {0}};
    const UmiUatProcessExpectedOutcome *outcomeTable = outcomes;
    size_t outcomeCount = sizeof outcomes / sizeof outcomes[0];
    const int *exit_table = expected_exits;
    size_t exit_count = sizeof expected_exits / sizeof expected_exits[0];
    const char *expected_message = NULL;
    int assertion_exit = 0;
    const uint32_t operations = UMI_UAT_WORKFLOW | UMI_UAT_UI_OBSERVE | UMI_UAT_UNIT;
    uint32_t allowed = operations;
    UmiStatus admission = UMI_STATUS_OK;
    UmiStatus observed = UMI_STATUS_OK;
    UmiCancellationToken *cancellation = NULL;
    CHECK(umi_cancellation_token_create(&cancellation) == UMI_STATUS_OK);
    int cancelled = 0;

    if (dual) {
        /* Choose approved fixtures for exact bytes, stream mismatches,
         * cancellation and table bounds without putting commands in the plan. */
        observation[1] = "dual-child";
        expected_message = "Process exit 7, stdout and stderr matched.";
        if (strcmp(mode, "dual-mismatch") == 0 || strcmp(mode, "dual-missing") == 0 ||
            strcmp(mode, "dual-unexpected") == 0 || strcmp(mode, "dual-long") == 0) {
            observed = UMI_STATUS_INVALID_STATE;
            assertion_exit = 7;
            expected_message = "Process stderr differed from the expected bytes.";
            if (strcmp(mode, "dual-mismatch") == 0) observation[1] = "dual-child-private";
            if (strcmp(mode, "dual-missing") == 0) observation[1] = "child-error";
            if (strcmp(mode, "dual-long") == 0) observation[1] = "dual-child-long";
            if (strcmp(mode, "dual-unexpected") == 0) outcomes[1] = (UmiUatProcessExpectedOutcome){.exitCode = 7};
        } else if (strcmp(mode, "dual-short-table") == 0) {
            --outcomeCount;
            admission = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(mode, "dual-limit") == 0) {
            outcomes[3].stderrBytes = expectedError;
            outcomes[3].stderrLength = UMI_UAT_PROCESS_EXPECTED_LIMIT + 1U;
            admission = UMI_STATUS_CAPACITY_EXCEEDED;
        } else if (strcmp(mode, "dual-null-span") == 0) {
            outcomes[3].stderrLength = 1U;
            admission = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(mode, "dual-null-table") == 0) {
            outcomeTable = NULL;
            admission = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(mode, "dual-code") == 0) {
            outcomes[3].exitCode = 128;
            admission = UMI_STATUS_INVALID_ARGUMENT;
        } else if (strcmp(mode, "dual-timeout") == 0) {
            observation[1] = "timeout";
            outcomes[1].exitCode = 124;
            steps[1].test.timeout_ms = 125U;
            observed = UMI_STATUS_TIMEOUT;
            expected_message = NULL;
        } else {
            CHECK(strcmp(mode, "dual-exact") == 0);
        }
    } else if (strcmp(mode, "target") == 0) {
        memcpy(target.sessionId, "different", sizeof "different");
        admission = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "denied") == 0) {
        allowed = UMI_UAT_WORKFLOW | UMI_UAT_UI_OBSERVE;
        admission = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "unmapped") == 0) {
        steps[3].test.enabled = 0;
        memcpy(steps[3].test.test_id, "missing", sizeof "missing");
        admission = UMI_STATUS_NOT_FOUND;
    } else if (strcmp(mode, "inline") == 0) {
        CHECK(umi_test_case_set_command(&steps[2].test, "unapproved", "extra", directory) == UMI_STATUS_OK);
        admission = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "deadline") == 0) {
        steps[2].test.timeout_ms = 5001U;
        admission = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "duplicate") == 0) {
        probes[3].testId = probes[2].testId;
        admission = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(mode, "trusted-path") == 0) {
        probes[3].workingDirectory = "relative";
        admission = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "argv") == 0) {
        observation[1] = NULL;
        admission = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "expected-limit") == 0) {
        probes[1].expectedStdoutLength = UMI_UAT_PROCESS_EXPECTED_LIMIT + 1U;
        admission = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "malformed-id") == 0) {
        memset(steps[3].test.test_id, 'x', sizeof steps[3].test.test_id);
        admission = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "cancel") == 0) {
        umi_cancellation_token_request(cancellation);
        cancelled = 1;
    } else if (strcmp(mode, "child-error") == 0) {
        observed = UMI_STATUS_INTERNAL_ERROR;
    } else if (strcmp(mode, "exit-expected") == 0) {
        observation[1] = "child-error";
        expected_message = "Process exit 7 and stdout matched.";
    } else if (strcmp(mode, "exit-different") == 0) {
        observation[1] = "child-error";
        expected_exits[1] = 8;
        observed = UMI_STATUS_INTERNAL_ERROR;
        assertion_exit = 7;
        expected_message = "Process exit was 7. Expected 8.";
    } else if (strcmp(mode, "exit-zero-mismatch") == 0) {
        observation[1] = "pass";
        observed = UMI_STATUS_INVALID_STATE;
        expected_message = "Process exit was 0. Expected 7.";
    } else if (strcmp(mode, "exit-stdout-mismatch") == 0) {
        observation[1] = "child-error";
        probes[1].expectedStdoutLength = sizeof expected - 1U;
        observed = UMI_STATUS_INVALID_STATE;
        assertion_exit = 7;
        expected_message = "Process stdout differed from the expected bytes.";
    } else if (strcmp(mode, "exit-short-table") == 0) {
        --exit_count;
        admission = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "exit-null-table") == 0) {
        exit_table = NULL;
        admission = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "exit-reserved-code") == 0) {
        expected_exits[3] = 137;
        admission = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "exit-timeout") == 0) {
        /* A host timeout stays a timeout even when a platform might report
         * the same numeric code as the probe's expected child result. */
        observation[1] = "timeout";
        expected_exits[1] = 124;
        steps[1].test.timeout_ms = 125U;
        observed = UMI_STATUS_TIMEOUT;
    } else if (strcmp(mode, "timeout") == 0) {
        steps[1].test.timeout_ms = 125U;
        observed = UMI_STATUS_TIMEOUT;
    } else if (strcmp(mode, "exact-tail") == 0 || strcmp(mode, "prefix") == 0 ||
               strcmp(mode, "mismatch") == 0 || strcmp(mode, "short") == 0) {
        observed = UMI_STATUS_INVALID_STATE;
    } else {
        CHECK(strcmp(mode, "pass") == 0 || strcmp(mode, "separate-streams") == 0);
    }

    char witnessName[128];
    char witnessPath[UMI_TEST_COMMAND_CAPACITY];
    const char *witness[] = {"--child", "witness", witnessName};
    if (admission != UMI_STATUS_OK || cancelled) {
        const int nameCount = snprintf(witnessName, sizeof witnessName, "uat-native-witness-%s", mode);
        CHECK(nameCount > 0 && (size_t)nameCount < sizeof witnessName);
        const int pathCount = snprintf(witnessPath, sizeof witnessPath, "%s/%s", directory, witnessName);
        CHECK(pathCount > 0 && (size_t)pathCount < sizeof witnessPath);
        NoWitness(witnessPath);
        probes[0].arguments = witness;
        probes[0].argumentCount = 3U;
    }

    UmiUatReport *report = NULL;
    /* The public routes use the same allowed target, actual processes and
     * no-launch witness. Select the owner's intended comparison. */
    if (dual) {
        CHECK(UmiUatRunProcessFixtureWithOutcomes(&target, allowed, &fixture, outcomeTable,
            outcomeCount, steps, count, cancellation, NULL, &report) == admission);
    } else if (explicit_exit) {
        CHECK(UmiUatRunProcessFixtureWithExitCodes(&target, allowed, &fixture, exit_table,
            exit_count, steps, count, cancellation, NULL, &report) == admission);
    } else {
        CHECK(UmiUatRunProcessFixture(&target, allowed, &fixture, steps, count,
            cancellation, NULL, &report) == admission);
    }
    umi_cancellation_token_destroy(cancellation);
    if (admission != UMI_STATUS_OK || cancelled) NoWitness(witnessPath);
    if (admission != UMI_STATUS_OK) {
        CHECK(report == NULL);
        return 0;
    }

    UmiUatSummary summary;
    UmiUatTargetEvidence evidence;
    UmiUatFinding finding;
    UmiUatOperation operation = 0;
    CHECK(UmiUatReportSummary(report, &summary) == UMI_STATUS_OK);
    CHECK(summary.planned == count && summary.cancellationRequested == cancelled);
    CHECK(UmiUatReportTarget(report, &evidence) == UMI_STATUS_OK);
    CHECK(evidence.target.platform == UMI_UAT_WEB && evidence.allowedOperations == operations &&
        evidence.supportedOperations == operations && strcmp(evidence.target.sessionId, "fixture-session") == 0);
    CHECK(UmiUatReportOperation(report, 1U, &operation) == UMI_STATUS_OK && operation == UMI_UAT_UI_OBSERVE);
    if (cancelled) {
        CHECK(!summary.accepted && summary.attempted == 0U && summary.notRun == count);
    } else {
        CHECK(UmiUatReportFinding(report, 1U, &finding) == UMI_STATUS_OK && finding.attempted);
        CHECK(finding.result.status == observed && strstr(finding.result.output, privateText) == NULL);
        if (expected_message != NULL) {
            CHECK(strcmp(finding.result.output, expected_message) == 0);
            CHECK(finding.result.exit_code == assertion_exit);
            /* The typed finding keeps its comparison. JSON includes that
             * message only when the caller explicitly requests outputs. */
            char json[4096];
            CHECK(UmiUatReportJson(report, 0, json, sizeof json) == UMI_STATUS_OK);
            CHECK(strstr(json, expected_message) == NULL);
            CHECK(UmiUatReportJson(report, 1, json, sizeof json) == UMI_STATUS_OK);
            CHECK(strstr(json, expected_message) != NULL && strstr(json, privateText) == NULL);
        }
        if (observed == UMI_STATUS_OK) {
            CHECK(summary.accepted && summary.passed == count);
            CHECK(strstr(finding.result.output, dual ? "stdout and stderr matched" : "stdout matched") != NULL);
        } else {
            CHECK(!summary.accepted && summary.attempted == 3U && summary.passed == 2U && summary.notRun == 1U);
            if (observed == UMI_STATUS_TIMEOUT) CHECK(finding.result.state == UMI_TEST_STATE_TIMED_OUT);
            if (observed == UMI_STATUS_INTERNAL_ERROR) CHECK(finding.result.exit_code == 7);
            if (observed == UMI_STATUS_INVALID_STATE && !explicit_exit && !dual)
                CHECK(finding.result.exit_code == 0 && strstr(finding.result.output, "stdout differed"));
            CHECK(UmiUatReportFinding(report, 3U, &finding) == UMI_STATUS_OK && finding.attempted &&
                finding.result.state == UMI_TEST_STATE_PASSED);
        }
    }
    UmiUatReportDestroy(report);
    return 0;
}
