/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_trader_cancel_reconcile.c
 *
 * PURPOSE:
 *   Execute and verify the trader.cancel-reconcile acceptance scenario.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    verify_journey("trader.cancel-reconcile", "org.umicom.trader", "trading");
    return 0;
}

