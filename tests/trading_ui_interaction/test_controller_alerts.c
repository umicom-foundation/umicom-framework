/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/trading_ui_interaction/test_controller_alerts.c
 *
 * PURPOSE:
 *   Verify that the guarded UI controller creates, activates, acknowledges and
 *   removes Framework-owned price alerts.
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

/* Verify price-alert creation, market activation and acknowledgement. */
void test_controller_alerts(void)
{
    UmiTradingUiTestFixture fixture;
    UmiTradingWorkspaceSnapshot snapshot;
    UmiTradingMarketSnapshot market;
    UmiTradingPriceAlert alert;
    double threshold;

    umi_trading_ui_test_fixture_init(&fixture, 1);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.has_selected_instrument && snapshot.has_quote);
    threshold = snapshot.selected_mid + 1.0;

    assert(umi_trading_ui_controller_create_price_alert(
               &fixture.controller,
               UMI_TRADING_PRICE_ALERT_CROSSES_ABOVE,
               threshold,
               1000) == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.alert_count == 1U);

    /* Pause and resume use the same guarded controller path as the panel. */
    assert(umi_trading_workspace_price_alert_at(
               fixture.workspace, 0U, &alert) == UMI_STATUS_OK);
    assert(umi_trading_ui_controller_set_price_alert_enabled(
               &fixture.controller, alert.alert_id, 0) == UMI_STATUS_OK);
    assert(umi_trading_workspace_price_alert_at(
               fixture.workspace, 0U, &alert) == UMI_STATUS_OK);
    assert(!alert.enabled);
    assert(umi_trading_ui_controller_set_price_alert_enabled(
               &fixture.controller, alert.alert_id, 1) == UMI_STATUS_OK);

    assert(umi_trading_workspace_selected_market(
               fixture.workspace, &market) == UMI_STATUS_OK);
    market.quote.bid = threshold + 1.0;
    market.quote.ask = threshold + 2.0;
    market.quote.event_time_ms += 100;
    assert(umi_trading_workspace_update_quote(
               fixture.workspace, &market.quote) == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.active_alert_count == 1U);
    assert(snapshot.unacknowledged_alert_count == 1U);
    assert(umi_trading_workspace_price_alert_at(
               fixture.workspace, 0U, &alert) == UMI_STATUS_OK);
    assert(alert.active && alert.occurrences == 1U);

    assert(umi_trading_ui_controller_acknowledge_price_alert(
               &fixture.controller, alert.alert_id) == UMI_STATUS_OK);
    assert(umi_trading_ui_controller_remove_price_alert(
               &fixture.controller, alert.alert_id) == UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(fixture.workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.alert_count == 0U);
    assert(fixture.changed_count == 5U);
    umi_trading_ui_test_fixture_destroy(&fixture);
}
