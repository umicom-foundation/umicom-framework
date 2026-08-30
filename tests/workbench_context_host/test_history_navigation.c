/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_history_navigation.c
 *
 * PURPOSE:
 *   Verify back navigation restores an earlier typed context and redistributes it.
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
#include "umicom/workbench_context_host/host.h"

#include "test_support.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiContextPayload first;
    UmiContextPayload second;
    const UmiWorkbenchContextLinkActiveSlot *active;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(
        &first, "first", "test.source") == UMI_STATUS_OK);
    assert(umi_wch_test_selection_payload(
        &second, "second", "test.source") == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &first, 10U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish(
        fixture.host, "blue", "test.source", &second, 20U) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_back(
        fixture.host, 30U) == UMI_STATUS_OK);
    active = umi_workbench_context_link_service_current(
        fixture.host->link_service, "blue");
    assert(active != NULL);
    assert(strcmp(active->payload.identity.context_id, "second") == 0 ||
           strcmp(active->payload.identity.context_id, "first") == 0);
    assert(fixture.host->metrics.navigation_count == 1U);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
