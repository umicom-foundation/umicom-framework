/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_host/test_trade_publisher.c
 *
 * PURPOSE:
 *   Verify canonical trade publication.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "test_support.h"
#include "umicom/workbench_context_host/trade_publisher.h"

int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    const UmiWorkbenchContextLinkActiveSlot *active;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_host_publish_trade(
        fixture.host, "red", "test.trading", "trade-context",
        "trade", "test", "future", "book", "counterparty", 3U, 100U) == UMI_STATUS_OK);
    active = umi_workbench_context_link_service_current(
        fixture.host->link_service, "red");
    assert(active != NULL);
    assert(active->payload.kind == UMI_CONTEXT_KIND_TRADE);
    assert(active->payload.identity.context_id[0] != '\0');
    umi_wch_test_fixture_destroy(&fixture);
    return 0;
}
