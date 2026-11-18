/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/uat/test_workflow.c
 *
 * PURPOSE:
 *   Verify general-purpose UAT prerequisite, assertion, cancellation and
 *   cleanup semantics without performing operations on owner resources.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include "umicom/testing/uat_workflow.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct Fixture {
    const char *mode;
    unsigned calls;
    unsigned cleanups;
    UmiCancellationToken *cancel;
    UmiUatStep *original;
} Fixture;

static UmiStatus Execute(void *context, const UmiTestCase *test,
    UmiCancellationToken *cancellation, UmiTestResult *result)
{
    Fixture *fixture = context;
    ++fixture->calls;
    if (strcmp(test->test_id, "cleanup") == 0) {
        ++fixture->cleanups;
        assert(cancellation != fixture->cancel);
    }
    umi_test_result_finish(result, UMI_STATUS_OK, 0, 1U);
    if (strcmp(fixture->mode, "setup-fails") == 0 && strcmp(test->test_id, "setup") == 0)
        umi_test_result_finish(result, UMI_STATUS_IO_ERROR, 1, 1U);
    if (strcmp(fixture->mode, "assert-fails") == 0 && strcmp(test->test_id, "assert") == 0)
        umi_test_result_finish(result, UMI_STATUS_OK, 1, 1U);
    if (strcmp(fixture->mode, "cleanup-fails") == 0 && strcmp(test->test_id, "cleanup") == 0)
        return UMI_STATUS_IO_ERROR;
    if (strcmp(fixture->mode, "double-failure") == 0 &&
        (strcmp(test->test_id, "assert") == 0 || strcmp(test->test_id, "cleanup") == 0))
        return UMI_STATUS_IO_ERROR;
    if (strcmp(fixture->mode, "cancel") == 0 && strcmp(test->test_id, "action") == 0)
        umi_cancellation_token_request(fixture->cancel);
    if (strcmp(fixture->mode, "cancel-last") == 0 && strcmp(test->test_id, "cleanup") == 0)
        umi_cancellation_token_request(fixture->cancel);
    if (strcmp(fixture->mode, "bad-adapter") == 0) result->state = UMI_TEST_STATE_RUNNING;
    if (strcmp(fixture->mode, "bad-output") == 0) memset(result->output, 'x', sizeof(result->output));
    if (strcmp(fixture->mode, "copy") == 0) memset(fixture->original, 0, 4U * sizeof(UmiUatStep));
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    const char *ids[] = {"setup", "action", "assert", "cleanup"};
    const UmiUatPhase phases[] = {UMI_UAT_SETUP, UMI_UAT_ACTION, UMI_UAT_ASSERT, UMI_UAT_CLEANUP};
    UmiUatStep steps[4] = {0};
    UmiUatReport *report = NULL;
    UmiUatSummary summary;
    UmiUatFinding finding;
    char json[8192];
    char tiny[2];
    Fixture fixture = {0};
    UmiStatus expected = UMI_STATUS_OK;
    assert(argc == 2);
    fixture.mode = argv[1];
    fixture.original = steps;
    assert(umi_cancellation_token_create(&fixture.cancel) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 4U; ++i) {
        steps[i].phase = phases[i];
        umi_test_case_init(&steps[i].test, ids[i], ids[i]);
        assert(umi_test_case_set_command(&steps[i].test, "fixture-adapter", "", "") == UMI_STATUS_OK);
        steps[i].test.timeout_ms = 1000U;
    }
    if (strcmp(fixture.mode, "duplicate") == 0) {
        memcpy(steps[1].test.test_id, steps[0].test.test_id, sizeof(steps[1].test.test_id));
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(fixture.mode, "phase-order") == 0) {
        steps[2].phase = UMI_UAT_SETUP;
        expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(fixture.mode, "no-assert") == 0) {
        steps[2].phase = UMI_UAT_ACTION;
        expected = UMI_STATUS_INVALID_STATE;
    } else if (strcmp(fixture.mode, "unterminated") == 0) {
        memset(steps[3].test.program, 'x', sizeof(steps[3].test.program));
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(fixture.mode, "zero-timeout") == 0) {
        steps[3].test.timeout_ms = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(fixture.mode, "disabled") == 0) steps[0].test.enabled = 0;
    else if (strcmp(fixture.mode, "cancel-before") == 0) umi_cancellation_token_request(fixture.cancel);
    assert(UmiUatRun(steps, 4U, Execute, &fixture, fixture.cancel, NULL, &report) == expected);
    if (expected != UMI_STATUS_OK) {
        assert(report == NULL && fixture.calls == 0U);
    } else {
        assert(UmiUatReportSummary(report, &summary) == UMI_STATUS_OK);
        assert(summary.planned == 4U);
        assert(summary.passed + summary.failed + summary.skipped + summary.notRun == 4U);
        if (strcmp(fixture.mode, "pass") == 0 || strcmp(fixture.mode, "copy") == 0) {
            assert(summary.accepted && summary.passed == 4U && fixture.cleanups == 1U);
        } else {
            assert(!summary.accepted);
            if (strcmp(fixture.mode, "setup-fails") == 0) {
                assert(summary.failed == 1U && summary.notRun == 2U && fixture.cleanups == 1U);
            } else if (strcmp(fixture.mode, "assert-fails") == 0) {
                assert(summary.failed == 1U && summary.passed == 3U && fixture.cleanups == 1U);
            } else if (strcmp(fixture.mode, "cleanup-fails") == 0) {
                assert(summary.failed == 1U && summary.cleanupFailed == 1U);
            } else if (strcmp(fixture.mode, "double-failure") == 0) {
                assert(summary.failed == 2U && summary.cleanupFailed == 1U);
                assert(UmiUatReportFinding(report, 2U, &finding) == UMI_STATUS_OK);
                assert(finding.result.status == UMI_STATUS_IO_ERROR);
            } else if (strcmp(fixture.mode, "cancel") == 0) {
                assert(summary.cancellationRequested && summary.notRun == 1U && fixture.cleanups == 1U);
            } else if (strcmp(fixture.mode, "cancel-last") == 0) {
                assert(summary.cancellationRequested && summary.passed == 4U);
            } else if (strcmp(fixture.mode, "cancel-before") == 0) {
                assert(summary.cancellationRequested && summary.notRun == 4U && fixture.calls == 0U);
            } else if (strcmp(fixture.mode, "disabled") == 0) {
                assert(summary.skipped == 1U && summary.notRun == 3U && fixture.calls == 0U);
            } else if (strcmp(fixture.mode, "bad-adapter") == 0 || strcmp(fixture.mode, "bad-output") == 0) {
                assert(summary.failed == 2U && summary.notRun == 2U && fixture.cleanups == 1U);
            } else assert(!"Unknown workflow fixture");
        }
        assert(UmiUatReportFinding(report, 4U, &finding) == UMI_STATUS_NOT_FOUND);
        assert(UmiUatReportJson(report, 0, json, sizeof(json)) == UMI_STATUS_OK);
        assert(strstr(json, "\"findings\"") != NULL);
        assert(strstr(json, "\"output\":") == NULL);
        assert(strstr(json, "\"cleanup\"") != NULL);
        assert(UmiUatReportJson(report, 0, tiny, sizeof(tiny)) == UMI_STATUS_CAPACITY_EXCEEDED);
        assert(tiny[0] == '\0');
        assert(UmiUatReportJson(report, 2, json, sizeof(json)) == UMI_STATUS_INVALID_ARGUMENT);
        assert(json[0] == '\0');
    }
    UmiUatReportDestroy(report);
    umi_cancellation_token_destroy(fixture.cancel);
    return 0;
}
