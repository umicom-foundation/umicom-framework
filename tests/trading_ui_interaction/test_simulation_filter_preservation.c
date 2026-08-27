/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_simulation_filter_preservation.c
 *
 * PURPOSE:
 *   Verify market animation preserves user watchlist filtering and selection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
#include <assert.h>
#include <string.h>

void test_simulation_filter_preservation(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingWorkspaceSnapshot before;
    UmiTradingWorkspaceSnapshot after;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_set_instrument_filter(
               &fixture.controller, "XAU") == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &before) ==
           UMI_STATUS_OK);
    assert(umi_trading_simulation_market_step(
               &fixture.simulation, 1000) == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &after) ==
           UMI_STATUS_OK);
    assert(strcmp(after.instrument_filter, "XAU") == 0);
    assert(after.visible_instrument_count == before.visible_instrument_count);
    assert(strcmp(after.selected_instrument_id, before.selected_instrument_id) == 0);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
