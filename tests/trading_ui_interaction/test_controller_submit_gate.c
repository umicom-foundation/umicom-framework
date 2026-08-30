/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_submit_gate.c
 *
 * PURPOSE:
 *   Verify submission is rejected until an explicit risk preview has passed.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
#include <assert.h>

void test_controller_submit_gate(void)
{
    UmiTradingUiTestFixture fixture;
    UmiRiskDecision decision = {0};
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_submit_order(
               &fixture.controller, 1001000, &decision) ==
           UMI_STATUS_INVALID_STATE);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
