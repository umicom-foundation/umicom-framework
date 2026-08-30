/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_preview.c
 *
 * PURPOSE:
 *   Verify explicit preview evaluates existing Framework pre-trade risk.
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

void test_controller_preview(void)
{
    UmiTradingUiTestFixture fixture;
    UmiRiskDecision decision = {0};
    UmiTradingWorkspaceSnapshot snapshot;
    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_ui_controller_select_instrument(
               &fixture.controller, "SIM.EURUSD") == UMI_STATUS_OK);
    assert(umi_trading_ui_controller_set_draft_quantity(
               &fixture.controller, 2.0) == UMI_STATUS_OK);
    assert(umi_trading_ui_controller_preview_order(
               &fixture.controller, &decision) == UMI_STATUS_OK);
    assert(decision.allowed);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.has_draft_risk);
    assert(snapshot.draft_risk.allowed);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
