/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_order_filter.c
 *
 * PURPOSE:
 *   Verify blotter filtering remains canonical workspace state.
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

/*
 * Exercise test controller order filter and return a clear result when the behaviour no
 * longer matches its contract.
 */
void test_controller_order_filter(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingWorkspaceSnapshot snapshot;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_set_order_filter(
               &fixture.controller,
               UMI_TRADING_WORKSPACE_ORDERS_OPEN) == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.order_filter == UMI_TRADING_WORKSPACE_ORDERS_OPEN);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
