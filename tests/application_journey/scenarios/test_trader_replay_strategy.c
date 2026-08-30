/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_trader_replay_strategy.c
 *
 * PURPOSE:
 *   Execute and verify the trader.replay-strategy acceptance scenario.
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
    verify_journey("trader.replay-strategy", "org.umicom.trader", "strategy-development");
    return 0;
}

