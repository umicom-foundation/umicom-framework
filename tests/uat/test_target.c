/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/uat/test_target.c
 *
 * PURPOSE:
 *   Verify target and operation admission using local adapters and check that
 *   the existing workflow keeps cleanup and refuses unsupported work safely.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/testing/uat_target.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); exit(EXIT_FAILURE); \
} } while (0)

typedef struct Fixture {
    UmiUatTarget target;
    UmiUatDriver driver;
    UmiUatStep steps[4];
    UmiUatOperation operations[4];
    UmiUatOperation assertion;
    UmiUatTargetPlatform platform;
    size_t calls;
    int modifyInputs;
    int failAssertion;
} Fixture;

static UmiStatus CheckBinding(void *context, const UmiUatTarget *target,
    UmiUatOperation operation, const UmiTestCase *test, UmiCancellationToken *token,
    UmiTestResult *result)
{
    Fixture *fixture = context;
    CHECK(token == NULL);
    CHECK(target->platform == fixture->platform && target->protocol == UMI_UAT_TARGET_PROTOCOL);
    CHECK(strcmp(target->targetId, "example.app") == 0);
    CHECK(strcmp(target->sessionId, "fixture-1") == 0);
    const char *expected = "close";
    UmiUatOperation expectedOperation = UMI_UAT_WORKFLOW;
    if (fixture->calls == 0U) expected = "open";
    if (fixture->calls == 1U) {
        expected = "observe";
        expectedOperation = fixture->assertion;
    }
    if (fixture->calls == 2U && !fixture->failAssertion) {
        expected = "verify";
        expectedOperation = UMI_UAT_UNIT;
    }
    CHECK(strcmp(test->test_id, expected) == 0 && operation == expectedOperation);
    if (fixture->modifyInputs && fixture->calls == 0U) {
        memset(&fixture->target, 0, sizeof(fixture->target));
        memset(&fixture->driver, 0, sizeof(fixture->driver));
        memset(fixture->steps, 0, sizeof(fixture->steps));
        memset(fixture->operations, 0, sizeof(fixture->operations));
    }
    ++fixture->calls;
    const int failed = fixture->failAssertion && strcmp(expected, "observe") == 0;
    umi_test_result_finish(result, failed ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK, failed, 0U);
    return UMI_STATUS_OK;
}

static uint32_t Configure(Fixture *fixture, UmiUatTargetPlatform platform, UmiUatOperation assertion)
{
    static const UmiUatPhase phases[] = {UMI_UAT_SETUP, UMI_UAT_ASSERT, UMI_UAT_ASSERT, UMI_UAT_CLEANUP};
    static const char *const ids[] = {"open", "observe", "verify", "close"};
    const UmiUatTarget target = {UMI_UAT_TARGET_PROTOCOL, platform, "example.app", "fixture-1"};
    const uint32_t permissions = UMI_UAT_WORKFLOW | UMI_UAT_UNIT | (uint32_t)assertion;
    fixture->target = target;
    fixture->driver = (UmiUatDriver){target, permissions, CheckBinding, fixture};
    fixture->platform = platform;
    fixture->assertion = assertion;
    fixture->operations[0] = UMI_UAT_WORKFLOW;
    fixture->operations[1] = assertion;
    fixture->operations[2] = UMI_UAT_UNIT;
    fixture->operations[3] = UMI_UAT_WORKFLOW;
    for (size_t i = 0; i < 4U; ++i) {
        fixture->steps[i].phase = phases[i];
        UmiTestCase *test = &fixture->steps[i].test;
        umi_test_case_init(test, ids[i], ids[i]);
        CHECK(umi_test_case_set_command(test, "local-fixture", "", "") == UMI_STATUS_OK);
        test->timeout_ms = 1000U;
    }
    return permissions;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    UmiUatTargetPlatform platform = UMI_UAT_DESKTOP;
    UmiUatOperation assertion = UMI_UAT_UI_OBSERVE;
    if (strcmp(mode, "mobile") == 0) {
        platform = UMI_UAT_MOBILE;
        assertion = UMI_UAT_UI_INTERACT;
    } else if (strcmp(mode, "web") == 0) {
        platform = UMI_UAT_WEB;
        assertion = UMI_UAT_SECURITY_ASSERT;
    } else if (strcmp(mode, "embedded") == 0) {
        platform = UMI_UAT_EMBEDDED;
        assertion = UMI_UAT_UNIT;
    }
    Fixture fixture = {0};
    uint32_t permissions = Configure(&fixture, platform, assertion);
    const uint32_t supported = permissions | UMI_UAT_DATABASE_READ;
    fixture.driver.supportedOperations = supported;
    /* The report copies only the visible identifier, including when a caller
     * reused a buffer that still contains bytes from a previous fixture. */
    fixture.driver.binding.targetId[UMI_UAT_TARGET_NAME_CAPACITY - 1U] = 'x';
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "detached-session") == 0) {
        fixture.driver.binding.sessionId[0] = 'F';
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "detached-platform") == 0) {
        fixture.driver.binding.platform = UMI_UAT_MOBILE;
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "detached-target") == 0) {
        fixture.driver.binding.targetId[0] = 'E';
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "invalid-identity") == 0) {
        fixture.target.targetId[1] = '/';
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "malformed-identity") == 0) {
        memset(fixture.target.sessionId, 'x', sizeof(fixture.target.sessionId));
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "revision") == 0) {
        ++fixture.driver.binding.protocol;
        expected = UMI_STATUS_UNAVAILABLE;
    } else if (strcmp(mode, "mask") == 0) {
        permissions |= UINT32_C(0x80000000);
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "unknown-operation") == 0) {
        fixture.operations[2] = (UmiUatOperation)128U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "ambiguous-operation") == 0) {
        fixture.operations[2] = (UmiUatOperation)(UMI_UAT_DATABASE_WRITE | UMI_UAT_UNIT);
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "disallowed-operation") == 0) {
        fixture.driver.supportedOperations |= UMI_UAT_DATABASE_WRITE;
        fixture.operations[1] = UMI_UAT_DATABASE_WRITE;
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "unsupported-operation") == 0) {
        fixture.driver.supportedOperations = UMI_UAT_WORKFLOW;
        expected = UMI_STATUS_UNAVAILABLE;
    } else if (strcmp(mode, "no-partial") == 0) {
        fixture.steps[3].test.enabled = 0;
        fixture.operations[3] = UMI_UAT_DATABASE_WRITE;
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(mode, "duplicate") == 0) {
        fixture.steps[2].test = fixture.steps[1].test;
        expected = UMI_STATUS_ALREADY_EXISTS;
    } else if (strcmp(mode, "copied-scope") == 0) {
        fixture.modifyInputs = 1;
    } else if (strcmp(mode, "cleanup") == 0) {
        fixture.failAssertion = 1;
    } else {
        CHECK(strcmp(mode, "desktop") == 0 || platform != UMI_UAT_DESKTOP);
    }

    UmiUatReport *report = (UmiUatReport *)(void *)&fixture;
    CHECK(UmiUatRunTarget(&fixture.target, permissions, &fixture.driver, fixture.steps,
        fixture.operations, 4U, NULL, NULL, &report) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(report == NULL && fixture.calls == 0U);
        return EXIT_SUCCESS;
    }
    UmiUatSummary summary;
    CHECK(UmiUatReportSummary(report, &summary) == UMI_STATUS_OK);
    CHECK(summary.planned == 4U && summary.accepted == !fixture.failAssertion);
    UmiUatTargetEvidence evidence;
    CHECK(UmiUatReportTarget(report, &evidence) == UMI_STATUS_OK);
    CHECK(evidence.target.protocol == UMI_UAT_TARGET_PROTOCOL && evidence.target.platform == platform);
    CHECK(strcmp(evidence.target.targetId, "example.app") == 0);
    CHECK(strcmp(evidence.target.sessionId, "fixture-1") == 0);
    CHECK(evidence.target.targetId[UMI_UAT_TARGET_NAME_CAPACITY - 1U] == '\0');
    CHECK(evidence.allowedOperations == permissions && evidence.supportedOperations == supported);
    const UmiUatOperation expectedOperations[] = {UMI_UAT_WORKFLOW, assertion,
        UMI_UAT_UNIT, UMI_UAT_WORKFLOW};
    UmiUatOperation operation;
    for (size_t i = 0U; i < 4U; ++i) {
        CHECK(UmiUatReportOperation(report, i, &operation) == UMI_STATUS_OK);
        CHECK(operation == expectedOperations[i]);
    }
    CHECK(UmiUatReportOperation(report, 4U, &operation) == UMI_STATUS_NOT_FOUND && operation == 0);
    CHECK(UmiUatReportOperation(NULL, 0U, &operation) == UMI_STATUS_INVALID_ARGUMENT && operation == 0);
    CHECK(UmiUatReportTarget(NULL, &evidence) == UMI_STATUS_INVALID_ARGUMENT && evidence.target.protocol == 0U);
    if (fixture.failAssertion) {
        UmiUatFinding notRun, cleanup;
        CHECK(summary.attempted == 3U && summary.failed == 1U && summary.notRun == 1U);
        CHECK(UmiUatReportFinding(report, 2U, &notRun) == UMI_STATUS_OK && !notRun.attempted);
        CHECK(UmiUatReportFinding(report, 3U, &cleanup) == UMI_STATUS_OK);
        CHECK(cleanup.phase == UMI_UAT_CLEANUP && cleanup.result.state == UMI_TEST_STATE_PASSED);
    } else {
        CHECK(summary.attempted == 4U && summary.passed == 4U);
    }
    CHECK(fixture.calls == summary.attempted);
    UmiUatReportDestroy(report);
    return EXIT_SUCCESS;
}
