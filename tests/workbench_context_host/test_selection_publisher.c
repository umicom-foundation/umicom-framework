/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_selection_publisher.c
 *
 * PURPOSE:
 *   Verify canonical generic selection publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "test_support.h"
#include "umicom/workbench_context_host/selection_publisher.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    const UmiWorkbenchContextLinkActiveSlot *active;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish_selection(
        fixture.host, "blue", "test.source", "selection-context",
        "selection", "file", "main.c", "secondary", 1U, 2U, 100U) == UMI_STATUS_OK);
    active = umi_workbench_context_link_service_current(
        fixture.host->link_service, "blue");
    assert(active != NULL);
    assert(active->payload.kind == UMI_CONTEXT_KIND_SELECTION);
    assert(active->payload.identity.context_id[0] != '\0');
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
