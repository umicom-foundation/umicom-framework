/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_selection.c
 *
 * PURPOSE:
 *   Verify explicit instrument selection updates shared trading context.
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
#include <string.h>

void test_controller_selection(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingWorkspaceSnapshot snapshot;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_select_instrument(
               &fixture.controller, "SIM.XAUUSD") == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(strcmp(snapshot.selected_instrument_id, "SIM.XAUUSD") == 0);
    assert(snapshot.has_selected_instrument);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
