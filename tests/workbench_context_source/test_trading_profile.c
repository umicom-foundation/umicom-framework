/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_source/test_trading_profile.c
 *
 * PURPOSE:
 *   Verify the reusable trading profile registers watchlist, chart, account, risk and TMS trade sources.
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
#include "umicom/workbench_context_source/workbench_context_source.h"


/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWorkbenchContextHostTestFixture fixture;
    UmiWorkbenchContextEventService *events = NULL;
    UmiWorkbenchContextSourceService *sources = NULL;
    assert(umi_wch_test_fixture_init(&fixture) == UMI_STATUS_OK);
    assert(umi_workbench_context_event_service_create(
        fixture.host, &events) == UMI_STATUS_OK);
    assert(umi_workbench_context_source_service_create(
        events, &sources) == UMI_STATUS_OK);

    UmiWorkbenchContextSourceTradingProfile profile =
        umi_workbench_context_source_trading_profile_default();
    assert(umi_workbench_context_source_register_trading_profile(
        sources, &profile) == UMI_STATUS_OK);
    assert(sources->registry.count == 5U);
    assert(umi_workbench_context_source_service_find(
        sources, "trader.watchlist.selection") != NULL);
    assert(umi_workbench_context_source_service_find(
        sources, "trader.account.selection") != NULL);
    assert(umi_workbench_context_source_service_find(
        sources, "tms.trade.selection") != NULL);
    umi_workbench_context_source_service_destroy(sources);
    umi_workbench_context_event_service_destroy(events);
    umi_wch_test_fixture_destroy(&fixture);

    return 0;
}
