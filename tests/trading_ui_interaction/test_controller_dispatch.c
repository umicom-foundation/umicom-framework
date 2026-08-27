/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_dispatch.c
 *
 * PURPOSE:
 *   Verify action-ID dispatch reaches typed Framework trading mutations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "test_support.h"
#include <assert.h>
#include <string.h>

void test_controller_dispatch(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingUiActionPayload payload = {0};
    UmiTradingWorkspaceSnapshot snapshot;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    payload.text = "XAU";
    assert(umi_trading_ui_controller_dispatch(
               &fixture.controller,
               UMI_TRADING_UI_ACTION_FILTER_INSTRUMENTS,
               &payload, NULL) == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(strcmp(snapshot.instrument_filter, "XAU") == 0);
    assert(snapshot.visible_instrument_count == 1U);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
