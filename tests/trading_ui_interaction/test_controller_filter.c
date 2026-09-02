/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_filter.c
 *
 * PURPOSE:
 *   Verify instrument filtering is delegated to the canonical workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

#include <assert.h>
#include <string.h>

/*
 * Exercise test controller filter and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_controller_filter(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingWorkspaceSnapshot snapshot;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_set_instrument_filter(
               &fixture.controller, "EUR") == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(strcmp(snapshot.instrument_filter, "EUR") == 0);
    assert(snapshot.visible_instrument_count >= 1U);
    assert(snapshot.visible_instrument_count < snapshot.watchlist_count);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
