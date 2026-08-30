/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_environment.c
 *
 * PURPOSE:
 *   Verify live environment selection is denied unless explicitly enabled.
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

void test_controller_environment(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingWorkspaceSnapshot snapshot;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_set_environment(
               &fixture.controller, UMI_TRADING_LIVE) ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.environment == UMI_TRADING_SIMULATION);
    assert(fixture.changed_count == 0U);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
