/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/uat/test_session_policy.c
 *
 * PURPOSE:
 *   Exercise account sign-out assertions and cleanup through a local UAT workflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/assertions.h"
#include "umicom/security/session.h"
#include "umicom/testing/uat_workflow.h"

#include <assert.h>
#include <string.h>

typedef struct SessionFixture {
    UmiSessionRegistry *registry;
    UmiSessionRecord first;
    UmiSessionRecord second;
    UmiSessionRecord other;
    int observe_only;
} SessionFixture;

static UmiStatus setup_sessions(SessionFixture *fixture)
{
    UmiStatus status = umi_session_registry_create(&fixture->registry);
    if (status == UMI_STATUS_OK) {
        status = umi_session_registry_issue(fixture->registry, "fixture-user", 10U, 20U, &fixture->first);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_session_registry_issue(fixture->registry, "fixture-user", 10U, 20U, &fixture->second);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_session_registry_issue(fixture->registry, "fixture-user-other", 10U, 20U, &fixture->other);
    }
    return status;
}

static UmiStatus check_session_policy(SessionFixture *fixture)
{
    UmiSessionRecord observed;
    if (umi_session_registry_validate(fixture->registry, fixture->first.session_id, 15U, &observed) != UMI_STATUS_PERMISSION_DENIED ||
        umi_session_registry_validate(fixture->registry, fixture->second.session_id, 15U, &observed) != UMI_STATUS_PERMISSION_DENIED ||
        umi_session_registry_validate(fixture->registry, fixture->other.session_id, 15U, &observed) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

/* This driver owns a memory-only registry. Its assertion checks the live
 * policy state after the action independently from the action's return code. */
static UmiStatus execute_session_step(void *context, const UmiTestCase *test,
                                     UmiCancellationToken *token, UmiTestResult *result)
{
    SessionFixture *fixture = context;
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    (void)token;
    if (strcmp(test->test_id, "issue") == 0) {
        status = setup_sessions(fixture);
    } else if (strcmp(test->test_id, "revoke") == 0) {
        size_t revoked = 0U;
        status = UMI_STATUS_OK;
        if (!fixture->observe_only) {
            status = umi_session_registry_revoke_principal(fixture->registry, "fixture-user", &revoked);
            if (status == UMI_STATUS_OK && revoked != 2U) status = UMI_STATUS_INVALID_STATE;
        }
    } else if (strcmp(test->test_id, "assert") == 0) {
        status = check_session_policy(fixture);
    } else if (strcmp(test->test_id, "cleanup") == 0) {
        umi_session_registry_destroy(fixture->registry);
        fixture->registry = NULL;
        status = UMI_STATUS_OK;
    }
    umi_test_result_finish(result, status, status == UMI_STATUS_OK ? 0 : 1, 0U);
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    SessionFixture fixture = {0};
    fixture.observe_only = strcmp(argv[1], "assertion-fails") == 0;
    assert(fixture.observe_only || strcmp(argv[1], "sign-out") == 0);

    const char *ids[] = {"issue", "revoke", "assert", "cleanup"};
    UmiUatStep steps[] = {{.phase = UMI_UAT_SETUP}, {.phase = UMI_UAT_ACTION},
                          {.phase = UMI_UAT_ASSERT}, {.phase = UMI_UAT_CLEANUP}};
    for (size_t i = 0U; i < sizeof(steps) / sizeof(steps[0]); ++i) {
        umi_test_case_init(&steps[i].test, ids[i], ids[i]);
        steps[i].test.kind = UMI_TEST_KIND_CONTRACT;
        steps[i].test.timeout_ms = 1000U;
        assert(umi_test_case_set_command(&steps[i].test, "local-session-policy", NULL, NULL) == UMI_STATUS_OK);
    }

    UmiUatReport *report = NULL;
    assert(UmiUatRun(steps, 4U, execute_session_step, &fixture, NULL, NULL, &report) == UMI_STATUS_OK);
    UmiUatSummary summary;
    UmiUatFinding finding;
    assert(UmiUatReportSummary(report, &summary) == UMI_STATUS_OK);
    assert(summary.attempted == 4U && summary.cleanupFailed == 0U && !fixture.registry);
    assert(summary.accepted == !fixture.observe_only && summary.failed == (size_t)fixture.observe_only);
    assert(UmiUatReportFinding(report, 2U, &finding) == UMI_STATUS_OK);
    assert(finding.result.status == (fixture.observe_only ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK));
    assert(UmiUatReportFinding(report, 3U, &finding) == UMI_STATUS_OK && finding.result.state == UMI_TEST_STATE_PASSED);
    UmiUatReportDestroy(report);
    return 0;
}
