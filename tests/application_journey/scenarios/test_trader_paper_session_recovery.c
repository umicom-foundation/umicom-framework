/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_trader_paper_session_recovery.c
 *
 * PURPOSE:
 *   Execute and verify the trader.paper-session-recovery acceptance scenario.
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
    verify_journey("trader.paper-session-recovery", "org.umicom.trader", "research");
    return 0;
}

