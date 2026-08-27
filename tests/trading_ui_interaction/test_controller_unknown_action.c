/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_unknown_action.c
 *
 * PURPOSE:
 *   Verify unknown UI actions fail closed instead of reaching trading state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
#include <assert.h>

void test_controller_unknown_action(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingUiControllerSnapshot snapshot;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_dispatch(
               &fixture.controller, "trading.unknown", NULL, NULL) ==
           UMI_STATUS_NOT_FOUND);
    snapshot = umi_trading_ui_controller_snapshot(&fixture.controller);
    assert(snapshot.last_status == UMI_STATUS_NOT_FOUND);
    assert(fixture.changed_count == 0U);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
