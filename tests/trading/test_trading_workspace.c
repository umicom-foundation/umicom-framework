/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_trading_workspace.c
 *
 * PURPOSE:
 *   Verify the professional trading coordinator composes deterministic market
 *   data, risk, orders, executions, positions and emergency-stop behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "test_trading_common.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiInstrument instrument = test_instrument();
    UmiQuote quote = {0};
    UmiBar bar = {0};
    UmiMarketDepth depth = {0};
    UmiTradingWorkspaceSnapshot snapshot;
    UmiTradingMarketSnapshot market;
    UmiRiskDecision decision;
    UmiExecutionReport execution = {0};
    UmiPosition position;

    assert(umi_trading_workspace_create(NULL, &workspace) == UMI_STATUS_OK);
    assert(umi_trading_workspace_add_instrument(workspace, &instrument) ==
           UMI_STATUS_OK);

    quote.instrument = instrument;
    quote.bid = 24999.0;
    quote.ask = 25001.0;
    quote.bid_size = 12.0;
    quote.ask_size = 10.0;
    quote.event_time_ms = 1000;
    assert(umi_trading_workspace_update_quote(workspace, &quote) ==
           UMI_STATUS_OK);

    bar.instrument = instrument;
    bar.open = 24900.0;
    bar.high = 25040.0;
    bar.low = 24880.0;
    bar.close = 25000.0;
    bar.volume = 3200.0;
    bar.start_time_ms = 0;
    bar.end_time_ms = 1000;
    assert(umi_trading_workspace_update_bar(workspace, &bar, 24950.0) ==
           UMI_STATUS_OK);

    depth.instrument = instrument;
    depth.bid_count = 2U;
    depth.ask_count = 2U;
    depth.bids[0].price = 24999.0;
    depth.bids[0].size = 12.0;
    depth.bids[1].price = 24998.0;
    depth.bids[1].size = 8.0;
    depth.asks[0].price = 25001.0;
    depth.asks[0].size = 10.0;
    depth.asks[1].price = 25002.0;
    depth.asks[1].size = 6.0;
    depth.event_time_ms = 1000;
    assert(umi_trading_workspace_update_depth(workspace, &depth) ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_set_market_state(
               workspace, instrument.instrument_id.value,
               UMI_MARKET_PREOPEN) == UMI_STATUS_OK);
    assert(umi_trading_workspace_set_market_state(
               workspace, instrument.instrument_id.value,
               UMI_MARKET_OPEN) == UMI_STATUS_OK);
    assert(umi_trading_workspace_set_health(workspace, 1, 0, 1) ==
           UMI_STATUS_OK);

    assert(umi_trading_workspace_set_instrument_filter(workspace, "CME") ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_select_instrument(
               workspace, instrument.instrument_id.value) == UMI_STATUS_OK);
    assert(umi_trading_workspace_selected_market(workspace, &market) ==
           UMI_STATUS_OK);
    assert(market.has_quote && market.has_bar && market.has_depth);

    assert(umi_trading_workspace_set_draft_quantity(workspace, 1.0) ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_set_draft_prices(workspace, 25000.0, 0.0) ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_preview_order(workspace, &decision) ==
           UMI_STATUS_OK);
    assert(decision.allowed);
    assert(umi_trading_workspace_submit_order(workspace, 2000, &decision) ==
           UMI_STATUS_OK);
    assert(decision.allowed);
    assert(umi_trading_workspace_snapshot(workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.order_count == 1U);
    assert(snapshot.can_cancel_order);

    (void)snprintf(execution.execution_id.value,
                   sizeof(execution.execution_id.value), "%s", "EXEC-1");
    (void)snprintf(execution.client_order_id.value,
                   sizeof(execution.client_order_id.value), "%s",
                   snapshot.selected_order_id);
    execution.fill_quantity = 1.0;
    execution.fill_price = 25000.0;
    execution.event_time_ms = 2100;
    assert(umi_trading_workspace_record_execution(workspace, &execution) ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_position_at(workspace, 0U, &position) ==
           UMI_STATUS_OK);
    assert(position.quantity == 1.0);
    assert(umi_trading_workspace_set_order_filter(
               workspace, UMI_TRADING_WORKSPACE_ORDERS_FILLED) ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_snapshot(workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.execution_count == 1U);
    assert(snapshot.position_count == 1U);
    assert(snapshot.visible_order_count == 1U);

    umi_trading_workspace_engage_kill_switch(workspace, "operator test");
    assert(umi_trading_workspace_snapshot(workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.kill_switch_engaged);
    assert(!snapshot.can_submit_order);
    assert(strcmp(snapshot.kill_switch_reason, "operator test") == 0);
    umi_trading_workspace_reset_kill_switch(workspace);
    assert(umi_trading_workspace_snapshot(workspace, &snapshot) ==
           UMI_STATUS_OK);
    assert(!snapshot.kill_switch_engaged);

    umi_trading_workspace_destroy(workspace);
    return 0;
}
