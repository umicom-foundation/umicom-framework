/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_trader_simulation_order.c
 *
 * PURPOSE:
 *   Execute and verify the trader.simulation-order acceptance scenario.
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
    verify_journey("trader.simulation-order", "org.umicom.trader", "trading");
    return 0;
}

