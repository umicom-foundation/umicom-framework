/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_session.c
 *
 * PURPOSE:
 *   Verify panel/group assignments round-trip through portable session text.
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
#include "test_support.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{

    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostSession session;
    UmiWorkbenchContextHostSession decoded;
    char text[UMI_WORKBENCH_CONTEXT_HOST_SESSION_TEXT_CAPACITY];
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_session_capture(
        fixture.host, &session) == UMI_STATUS_OK);
    assert(session.assignment_count == 3U);
    assert(umi_workbench_context_host_session_encode(
        &session, text, sizeof(text)) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_session_decode(
        text, &decoded) == UMI_STATUS_OK);
    assert(decoded.assignment_count == session.assignment_count);
    assert(strcmp(decoded.active_group_id, "blue") == 0);
    assert(umi_workbench_context_host_session_apply(
        fixture.host, &decoded) == UMI_STATUS_OK);
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
