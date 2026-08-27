/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_trader_kill_switch_recovery.c
 *
 * PURPOSE:
 *   Execute and verify the trader.kill-switch-recovery acceptance scenario.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("trader.kill-switch-recovery", "org.umicom.trader", "trading");
    return 0;
}

