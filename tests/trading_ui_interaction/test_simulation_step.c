/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_simulation_step.c
 *
 * PURPOSE:
 *   Verify deterministic simulation advancement updates market evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

#include <assert.h>

void test_simulation_step(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingMarketSnapshot before;
    UmiTradingMarketSnapshot after;
    uint64_t sequence;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_workspace_selected_market(fixture.workspace, &before) ==
           UMI_STATUS_OK);
    sequence = umi_trading_simulation_market_sequence(&fixture.simulation);
    assert(umi_trading_simulation_market_step(&fixture.simulation, 1000) ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_selected_market(fixture.workspace, &after) ==
           UMI_STATUS_OK);
    assert(umi_trading_simulation_market_sequence(&fixture.simulation) ==
           sequence + 1U);
    assert(after.quote.bid != before.quote.bid ||
           after.quote.ask != before.quote.ask);
    assert(after.revision > before.revision);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
