/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_simulation_idempotence.c
 *
 * PURPOSE:
 *   Verify simulation seeding cannot silently duplicate a market catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
#include <assert.h>

void test_simulation_idempotence(void)
{
    UmiTradingUiTestFixture fixture;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_simulation_market_seed_default(
               &fixture.simulation, 2000000) == UMI_STATUS_ALREADY_EXISTS);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
