/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_refresh.c
 *
 * PURPOSE:
 *   Verify refresh stays in the canonical workspace and publishes one change.
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

void test_controller_refresh(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingUiControllerSnapshot snapshot;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_refresh(&fixture.controller) == UMI_STATUS_OK);
    snapshot = umi_trading_ui_controller_snapshot(&fixture.controller);
    assert(snapshot.revision > 1U);
    assert(fixture.changed_count == 1U);
    assert(fixture.last_revision == snapshot.revision);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
