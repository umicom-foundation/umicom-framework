/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_journey/scenarios/test_trader_cancel_reconcile.c
 *
 * PURPOSE:
 *   Execute and verify the trader.cancel-reconcile acceptance scenario.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"
int main(void)
{
    verify_journey("trader.cancel-reconcile", "org.umicom.trader", "trading");
    return 0;
}

