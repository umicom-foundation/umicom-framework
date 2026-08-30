/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_trader_market_data_navigation.c
 *
 * PURPOSE:
 *   Execute and verify the trader.market-data-navigation acceptance scenario.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("trader.market-data-navigation", "org.umicom.trader", "trading");
    return 0;
}

