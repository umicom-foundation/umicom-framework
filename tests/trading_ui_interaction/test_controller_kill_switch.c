/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_kill_switch.c
 *
 * PURPOSE:
 *   Verify kill-switch actions remain owned by the canonical trading workspace.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"

#include <assert.h>

void test_controller_kill_switch(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingWorkspaceSnapshot snapshot;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_engage_kill_switch(
               &fixture.controller, "test stop") == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.kill_switch_engaged);
    assert(umi_trading_ui_controller_reset_kill_switch(
               &fixture.controller) == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(!snapshot.kill_switch_engaged);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
