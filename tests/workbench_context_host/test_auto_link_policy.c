/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_auto_link_policy.c
 *
 * PURPOSE:
 *   Verify role precedent selects a compatible default group.
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
#include "umicom/workbench_context_host/auto_link_policy.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextHostAutoLinkDecision decision;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_auto_link_decide(
        &fixture.profile,
        UMI_WORKBENCH_CONTEXT_HOST_PANEL_GENERIC,
        UMI_CONTEXT_KIND_PROJECT,
        &decision) == UMI_STATUS_OK);
    assert(decision.found);
    assert(strcmp(decision.group_id, "blue") == 0);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
