/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_simulation_seed.c
 *
 * PURPOSE:
 *   Verify the default simulation seeds usable quotes, bars and depth.
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

/*
 * Exercise test simulation seed and return a clear result when the behaviour no longer
 * matches its contract.
 */
void test_simulation_seed(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingWorkspaceSnapshot snapshot;
    UmiTradingMarketSnapshot market;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_simulation_market_instrument_count(&fixture.simulation) ==
           UMI_TRADING_SIMULATION_MARKET_DEFAULT_INSTRUMENTS);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.watchlist_count ==
           UMI_TRADING_SIMULATION_MARKET_DEFAULT_INSTRUMENTS);
    assert(snapshot.market_data_ready);
    assert(umi_trading_workspace_selected_market(fixture.workspace, &market) ==
           UMI_STATUS_OK);
    assert(market.has_quote);
    assert(market.has_bar);
    assert(market.has_depth);
    assert(market.depth.bid_count == 5U);
    assert(market.depth.ask_count == 5U);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
