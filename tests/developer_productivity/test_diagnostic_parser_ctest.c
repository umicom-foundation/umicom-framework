/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_productivity/test_diagnostic_parser_ctest.c
 *
 * PURPOSE:
 *   Verify the built-in CTest diagnostic parser recognizes representative
 *   tool output and emits a normalized problem.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>

#include "umicom/developer_productivity/diagnostic_parsers/ctest.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDeveloperProblem problem;
    int matched = 0;
    const UmiDeveloperDiagnosticParser *parser =
        umi_developer_diagnostic_parser_ctest();

    assert(umi_developer_diagnostic_parser_validate(parser) == UMI_STATUS_OK);
    assert(parser->parse(
        "Test #42: example ................................***Failed",
        &problem,
        &matched) == UMI_STATUS_OK);
    assert(matched == 1);
    assert(umi_developer_problem_validate(&problem) == UMI_STATUS_OK);
    /* Use distinct observed spellings, not a generated count of equivalent
     * tests. Assertions remain active through the existing fixture helper. */
    const struct {
        const char *line;
        const char *code;
        UmiDeveloperProductivitySeverity severity;
    } cases[] = {
        {"1/8 Test #42: unit.failed ....***Failed 0.10 sec", "ctest.failed", UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR},
        {"2/8 Test #43: unit.timeout ....***Timeout 60.00 sec", "ctest.timeout", UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR},
        {"3/8 Test #44: unit.missing ....***Not Run 0.00 sec", "ctest.not-run", UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR},
        {"4/8 Test #45: unit.crash ....Exit code 0xc0000409***Exception: 1.06 sec", "ctest.exception", UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR},
        {"5/8 Test #46: unit.signal ....***Exception: SegFault 0.10 sec", "ctest.exception", UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR},
        {"6/8 Test #47: unit.host ....***Skipped 0.10 sec", "ctest.skipped", UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING},
        {"Test #50: unit.disabled ....***Not Run (Disabled) 0.00 sec", "ctest.disabled", UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING},
        {"7/8 Test #48: unit.off ....***Disabled 0.00 sec", "ctest.disabled", UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_WARNING},
        {"\x1b[31m8/8 Test #49: spaced test name ....***Failed\x1b[0m\r\n", "ctest.failed", UMI_DEVELOPER_PRODUCTIVITY_SEVERITY_ERROR}
    };
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) {
        matched = 0;
        assert(parser->parse(cases[i].line, &problem, &matched) == UMI_STATUS_OK);
        assert(matched == 1 && strcmp(problem.code, cases[i].code) == 0);
        assert(problem.severity == cases[i].severity);
        assert(problem.location.uri[0] == '\0' && problem.location.line == 0U);
        assert(strchr(problem.message, '\x1b') == NULL);
        assert(umi_developer_problem_validate(&problem) == UMI_STATUS_OK);
    }
    const char *ignored[] = {
        "1/1 Test #42: failed.error.Timeout .... Passed 0.10 sec",
        "99% tests passed, 4 tests failed out of 100",
        "Start 42: example.failed", "The following tests FAILED:",
        "Failed output from a different tool", "1/1 Test #42: unit ....***FailedSuffix",
        "2/1 Test #42: unit ....***Failed", "0/1 Test #42: unit ....***Failed",
        "1/1 Test #0: unit ....***Failed", "1/1 Test #42: ....***Failed",
        "1/1 Test #42: unit ....Exit code 0x***Exception:",
        "18446744073709551616/2 Test #42: unit ....***Failed"
    };
    UmiDeveloperProblem previous;
    memcpy(&previous, &problem, sizeof previous);
    for (size_t i = 0U; i < sizeof ignored / sizeof ignored[0]; ++i) {
        matched = 99;
        assert(parser->parse(ignored[i], &problem, &matched) == UMI_STATUS_OK);
        assert(matched == 0 && memcmp(&problem, &previous, sizeof problem) == 0);
    }
    assert(parser->parse("\x1b[31", &problem, &matched) == UMI_STATUS_PARSE_ERROR);
    assert(matched == 0 && memcmp(&problem, &previous, sizeof problem) == 0);
    char large[UMI_DEVELOPER_PRODUCTIVITY_TEXT_CAPACITY + 1U];
    memset(large, 'x', sizeof large - 1U); large[sizeof large - 1U] = '\0';
    assert(parser->parse(large, &problem, &matched) == UMI_STATUS_CAPACITY_EXCEEDED);
    assert(matched == 0 && memcmp(&problem, &previous, sizeof problem) == 0);
    matched = 99;
    assert(parser->parse(NULL, &problem, &matched) == UMI_STATUS_INVALID_ARGUMENT);
    assert(matched == 99 && memcmp(&problem, &previous, sizeof problem) == 0);
    return 0;
}
