/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_trader_live_release_gate.c
 *
 * PURPOSE:
 *   Execute and verify the trader.live-release-gate acceptance scenario.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("trader.live-release-gate", "org.umicom.trader", "trading");
    return 0;
}

