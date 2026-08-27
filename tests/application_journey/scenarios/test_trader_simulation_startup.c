/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_trader_simulation_startup.c
 *
 * PURPOSE:
 *   Execute and verify the trader.simulation-startup acceptance scenario.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("trader.simulation-startup", "org.umicom.trader", "trading");
    return 0;
}

