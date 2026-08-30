/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_test_platform_structured_report.c
 *
 * PURPOSE:
 *   Verify deterministic escaped JSON for results and session summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/test_platform/test_platform.h"

int main(void)
{
    UmiTestPlatformResultSnapshot result;
    UmiTestPlatformRunSessionSnapshot session;
    char json[8192];
    (void)memset(&result, 0, sizeof(result));
    (void)strcpy(result.id, "result.1");
    (void)strcpy(result.session_id, "session.1");
    (void)strcpy(result.item_id, "framework.alpha");
    (void)strcpy(result.message, "assertion failed");
    (void)strcpy(result.failure_details, "expected \"yes\"\nactual no");
    result.outcome = UMI_TEST_PLATFORM_OUTCOME_FAILED;
    result.duration_ms = 12.5;
    result.exit_code = 1;
    result.sequence = 7U;
    assert(umi_test_platform_report_result_json(&result, json, sizeof(json)) ==
           UMI_STATUS_OK);
    assert(strstr(json, "\"outcome\":\"failed\"") != NULL);
    assert(strstr(json, "expected \\\"yes\\\"\\nactual no") != NULL);

    (void)memset(&session, 0, sizeof(session));
    (void)strcpy(session.id, "session.1");
    (void)strcpy(session.profile_id, "profile.default");
    (void)strcpy(session.suite_id, "framework.ctest");
    session.total = 3U;
    session.passed = 2U;
    session.failed = 1U;
    assert(umi_test_platform_report_session_json(&session, json,
                                                sizeof(json)) ==
           UMI_STATUS_OK);
    assert(strstr(json, "\"total\":3") != NULL);
    assert(strstr(json, "\"failed\":1") != NULL);
    return 0;
}
