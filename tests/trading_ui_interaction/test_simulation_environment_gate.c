/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_simulation_environment_gate.c
 *
 * PURPOSE:
 *   Verify deterministic simulation never advances paper or live environments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
#include <assert.h>

void test_simulation_environment_gate(void)
{
    UmiTradingUiTestFixture fixture;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_set_environment(
               &fixture.controller, UMI_TRADING_PAPER) == UMI_STATUS_OK);
    assert(umi_trading_simulation_market_step(
               &fixture.simulation, 1000) == UMI_STATUS_INVALID_STATE);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
