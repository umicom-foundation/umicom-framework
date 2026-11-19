/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/uat/test_process_arguments.c
 *
 * PURPOSE:
 *   Check the shared process runner and UAT against the arguments received by a child.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/process.h"
#include "umicom/testing/runner.h"
#include "umicom/testing/uat_workflow.h"

#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } \
} while (0)

static int Child(int argc, char **argv)
{
    if (strcmp(argv[1], "child-quoted") == 0) {
        CHECK(argc == 5);
        CHECK(strcmp(argv[2], "two words") == 0);
        CHECK(argv[3][0] == '\0');
        CHECK(strcmp(argv[4], "C:\\work\\test") == 0);
    } else if (strcmp(argv[1], "child-limit") == 0) {
        CHECK((size_t)argc == UMI_PROCESS_MAX_ARGUMENTS + 1U);
        for (int i = 2; i < argc - 1; ++i) CHECK(strcmp(argv[i], "x") == 0);
        CHECK(strcmp(argv[argc - 1], "last") == 0);
    }
    puts("process fixture executed");
    return 0;
}

static void AtLimit(char *arguments, size_t capacity, int overflow)
{
    size_t used = (size_t)snprintf(arguments, capacity, "child-limit");
    for (size_t i = 2U; i < UMI_PROCESS_MAX_ARGUMENTS; ++i) {
        used += (size_t)snprintf(arguments + used, capacity - used, " x");
    }
    (void)snprintf(arguments + used, capacity - used, "%s", overflow ? " last extra" : " last\t ");
}

static UmiStatus SetCase(UmiTestCase *test, const char *id, const char *program, const char *arguments)
{
    umi_test_case_init(test, id, id);
    test->timeout_ms = 3000U;
    return umi_test_case_set_command(test, program, arguments, NULL);
}

static int Workflow(const char *program)
{
    UmiUatStep steps[3] = {{.phase = UMI_UAT_ACTION}, {.phase = UMI_UAT_ASSERT}, {.phase = UMI_UAT_CLEANUP}};
    UmiUatSummary summary;
    UmiUatFinding finding;
    UmiUatReport *report = NULL;
    char arguments[128U + 2U * UMI_PROCESS_MAX_ARGUMENTS];
    AtLimit(arguments, sizeof(arguments), 1);
    CHECK(SetCase(&steps[0].test, "action", program, arguments) == UMI_STATUS_OK);
    CHECK(SetCase(&steps[1].test, "assert", program, "child-marker") == UMI_STATUS_OK);
    CHECK(SetCase(&steps[2].test, "cleanup", program, "child-marker") == UMI_STATUS_OK);
    CHECK(UmiUatRun(steps, 3U, UmiUatExecuteProcess, NULL, NULL, NULL, &report) == UMI_STATUS_OK);
    CHECK(UmiUatReportSummary(report, &summary) == UMI_STATUS_OK);
    CHECK(summary.attempted == 2U && summary.failed == 1U && summary.passed == 1U && summary.notRun == 1U);
    CHECK(!summary.accepted && summary.cleanupFailed == 0U);
    CHECK(UmiUatReportFinding(report, 0U, &finding) == UMI_STATUS_OK && finding.attempted);
    CHECK(finding.result.status == UMI_STATUS_CAPACITY_EXCEEDED && finding.result.exit_code == -1);
    CHECK(strstr(finding.result.output, "process fixture executed") == NULL);
    CHECK(UmiUatReportFinding(report, 1U, &finding) == UMI_STATUS_OK && !finding.attempted);
    CHECK(UmiUatReportFinding(report, 2U, &finding) == UMI_STATUS_OK && finding.attempted);
    CHECK(strstr(finding.result.output, "process fixture executed") != NULL);
    UmiUatReportDestroy(report);
    return 0;
}

int main(int argc, char **argv)
{
    UmiTestCase test;
    UmiTestResult result;
    char arguments[128U + 2U * UMI_PROCESS_MAX_ARGUMENTS];
    UmiStatus expected = UMI_STATUS_OK;

    if (argc >= 2 && strncmp(argv[1], "child-", 6U) == 0) return Child(argc, argv);
    if (argc != 2) return 2;
    const char *scenario = argv[1];
    if (strcmp(scenario, "workflow") == 0) return Workflow(argv[0]);
    const char *text = "  child-quoted\t\"two words\" \"\" C:\\work\\test  ";
    if (strcmp(scenario, "exact") == 0 || strcmp(scenario, "overflow") == 0) {
        const int overflow = strcmp(scenario, "overflow") == 0;
        AtLimit(arguments, sizeof(arguments), overflow);
        text = arguments;
        if (overflow) expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(scenario, "open-quote") == 0) {
        text = "child-marker \"open argument";
        expected = UMI_STATUS_PARSE_ERROR;
    } else if (strcmp(scenario, "fixed-field") == 0) {
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(scenario, "quoted") != 0) {
        return 2;
    }
    CHECK(SetCase(&test, "arguments", argv[0], text) == UMI_STATUS_OK);
    if (strcmp(scenario, "fixed-field") == 0) {
        memset(test.argument_text, 'x', sizeof(test.argument_text));
    }
    CHECK(umi_test_runner_run_case(&test, NULL, &result) == expected);
    CHECK(result.status == expected);
    if (expected == UMI_STATUS_OK) {
        CHECK(result.state == UMI_TEST_STATE_PASSED && result.exit_code == 0);
        CHECK(strstr(result.output, "process fixture executed") != NULL);
    } else {
        CHECK(result.state == UMI_TEST_STATE_FAILED && result.exit_code == -1);
        CHECK(strstr(result.output, "process fixture executed") == NULL);
    }
    return 0;
}
