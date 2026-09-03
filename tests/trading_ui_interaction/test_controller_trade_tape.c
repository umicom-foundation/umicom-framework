/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_trade_tape.c
 *
 * PURPOSE:
 *   Verify shared Time and Sales actions filter and pause the visible tape
 *   without stopping deterministic market ingestion.
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

/* Exercise the toolkit-neutral controller because native and model-driven
 * panels both route their buttons through this same policy boundary. */
void test_controller_trade_tape(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingUiActionPayload payload = {0};
    UmiTradingWorkspaceSnapshot before;
    UmiTradingWorkspaceSnapshot paused;
    UmiTradingWorkspaceSnapshot resumed;
    UmiTradingUiControllerSnapshot controller_snapshot;

    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &before) ==
           UMI_STATUS_OK);
    assert(before.selected_trade_count == 1U);

    payload.primary_value = UMI_TRADING_TRADE_TAPE_ALL;
    payload.primary_number = 1.0;
    assert(umi_trading_ui_controller_dispatch(
               &fixture.controller,
               UMI_TRADING_UI_ACTION_FILTER_TRADE_TAPE,
               &payload,
               NULL) == UMI_STATUS_OK);
    assert(umi_trading_ui_controller_dispatch(
               &fixture.controller,
               UMI_TRADING_UI_ACTION_PAUSE_TRADE_TAPE,
               NULL,
               NULL) == UMI_STATUS_OK);
    assert(umi_trading_simulation_market_step(&fixture.simulation, 1000) ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &paused) ==
           UMI_STATUS_OK);
    assert(paused.trade_tape.paused);
    assert(paused.trade_tape.retained_count > before.trade_tape.retained_count);
    assert(paused.selected_trade_count == before.selected_trade_count);

    assert(umi_trading_ui_controller_dispatch(
               &fixture.controller,
               UMI_TRADING_UI_ACTION_RESUME_TRADE_TAPE,
               NULL,
               NULL) == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &resumed) ==
           UMI_STATUS_OK);
    assert(!resumed.trade_tape.paused);
    assert(resumed.selected_trade_count == before.selected_trade_count + 1U);
    controller_snapshot = umi_trading_ui_controller_snapshot(
        &fixture.controller);
    assert(strstr(controller_snapshot.last_message, "resumed") != NULL);
    assert(fixture.changed_count >= 3U);

    umi_trading_ui_test_fixture_destroy(&fixture);
}
