/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_draft.c
 *
 * PURPOSE:
 *   Verify order-entry fields mutate only the canonical guarded draft order.
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

void test_controller_draft(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingWorkspaceSnapshot snapshot;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_select_instrument(
               &fixture.controller, "SIM.XAUUSD") == UMI_STATUS_OK);
    assert(umi_trading_ui_controller_set_draft_side(
               &fixture.controller, UMI_SIDE_SELL) == UMI_STATUS_OK);
    assert(umi_trading_ui_controller_set_draft_type(
               &fixture.controller, UMI_ORDER_LIMIT, UMI_TIF_GTC) ==
           UMI_STATUS_OK);
    assert(umi_trading_ui_controller_set_draft_quantity(
               &fixture.controller, 5.0) == UMI_STATUS_OK);
    assert(umi_trading_ui_controller_set_draft_prices(
               &fixture.controller, 3396.0, 0.0) == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.draft_order.side == UMI_SIDE_SELL);
    assert(snapshot.draft_order.type == UMI_ORDER_LIMIT);
    assert(snapshot.draft_order.tif == UMI_TIF_GTC);
    assert(snapshot.draft_order.quantity == 5.0);
    assert(snapshot.draft_order.limit_price == 3396.0);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
