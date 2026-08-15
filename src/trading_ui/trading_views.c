/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_ui/trading_views.c
 *
 * PURPOSE:
 *   Project the safe professional trading workspace into reusable view models
 *   for dashboards, watchlists, depth, charts, order entry, order history,
 *   executions, portfolio positions and risk controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading_ui/trading_ui.h"

#include <stdio.h>
#include <string.h>

#include "umicom/ui/command_view.h"

/* View construction is intentionally data-only. A GUI adapter may render the
 * properties as tables, cards or native controls without changing trading
 * policy or duplicating order-safety decisions. */
static size_t visible_rows(size_t count)
{
    return count < UMI_TRADING_UI_VISIBLE_ROWS
        ? count : UMI_TRADING_UI_VISIBLE_ROWS;
}

static UmiStatus set_string(UmiUiViewModel *view, const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(
        &value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_integer(UmiUiViewModel *view, const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_number(UmiUiViewModel *view, const char *key,
                            double number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_real(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_boolean(UmiUiViewModel *view, const char *key,
                             int enabled)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, enabled);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

static UmiStatus set_action(UmiUiViewModel *view, size_t index,
                            const char *action_id, const char *label,
                            const char *tooltip, int enabled)
{
    UmiUiCommandViewAction action = {0};

    (void)snprintf(action.action_id, sizeof(action.action_id), "%s",
                   action_id);
    (void)snprintf(action.label, sizeof(action.label), "%s", label);
    (void)snprintf(action.tooltip, sizeof(action.tooltip), "%s", tooltip);
    action.enabled = enabled != 0;
    return umi_ui_command_view_set_action(view, index, &action);
}

static UmiStatus create_view(const char *view_id, const char *view_kind,
                             const char *title, const char *summary,
                             UmiUiViewModel **out_view)
{
    UmiStatus status;

    if (view_id == NULL || view_kind == NULL || out_view == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_view_model_create(view_id, "umicom.trading-ui",
                                      UMI_UI_ROLE_PANE, out_view);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "umicom.view-kind", view_kind);
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "summary", summary);
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

static UmiStatus set_workspace_properties(
    UmiUiViewModel *view, const UmiTradingWorkspaceSnapshot *snapshot)
{
    UmiStatus status = set_integer(
        view, "trading.revision", (int64_t)snapshot->revision);
    if (status == UMI_STATUS_OK)
        status = set_string(view, "trading.account-id",
                            snapshot->account_id.value);
    if (status == UMI_STATUS_OK)
        status = set_string(view, "trading.environment",
            umi_trading_environment_text(snapshot->environment));
    if (status == UMI_STATUS_OK)
        status = set_string(view, "trading.selected-instrument",
                            snapshot->selected_instrument_id);
    if (status == UMI_STATUS_OK)
        status = set_string(view, "trading.selected-order",
                            snapshot->selected_order_id);
    if (status == UMI_STATUS_OK)
        status = set_boolean(view, "trading.market-data-ready",
                             snapshot->market_data_ready);
    if (status == UMI_STATUS_OK)
        status = set_boolean(view, "trading.broker-ready",
                             snapshot->broker_ready);
    if (status == UMI_STATUS_OK)
        status = set_boolean(view, "trading.risk-ready",
                             snapshot->risk_ready);
    if (status == UMI_STATUS_OK)
        status = set_boolean(view, "trading.health-ready",
                             snapshot->health_ready);
    if (status == UMI_STATUS_OK)
        status = set_boolean(view, "trading.kill-switch-engaged",
                             snapshot->kill_switch_engaged);
    return status;
}

static UmiStatus take_snapshot(UmiTradingWorkspace *workspace,
                               UmiTradingWorkspaceSnapshot *out_snapshot)
{
    if (workspace == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_trading_workspace_snapshot(workspace, out_snapshot);
}

UmiStatus umi_trading_ui_dashboard_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-dashboard", "Trading Dashboard",
        "Environment safety, market health, activity and exposure at a glance.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.instrument-count",
                             (int64_t)snapshot.watchlist_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.order-count",
                             (int64_t)snapshot.order_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.execution-count",
                             (int64_t)snapshot.execution_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.position-count",
                             (int64_t)snapshot.position_count);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.gross-position-quantity",
                            snapshot.gross_position_quantity);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.realised-pnl",
                            snapshot.realised_pnl);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.selected-mid",
                            snapshot.selected_mid);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.selected-change-percent",
                            snapshot.selected_change_percent);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh",
        "Reconcile the trading workspace snapshot", 1);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.set-environment", "Environment…",
        "Choose simulation, paper or live with explicit safety gates", 1);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.trading.engage-kill-switch", "Stop Trading",
        "Engage the workspace kill switch immediately",
        !snapshot.kill_switch_engaged);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.trading.reset-kill-switch", "Reset Stop",
        "Reset the kill switch after the cause has been reviewed",
        snapshot.can_reset_kill_switch);
    return status;
}

UmiStatus umi_trading_ui_watchlist_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    size_t count;
    size_t index;
    UmiStatus status = create_view(
        view_id, "trading-watchlist", "Watchlist",
        "Filter and select canonical instruments with live market evidence.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "trading.instrument-filter",
                            snapshot.instrument_filter);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.visible-instrument-count",
                             (int64_t)snapshot.visible_instrument_count);
    count = visible_rows(snapshot.visible_instrument_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTradingMarketSnapshot market;
        char key[64];
        char text[480];
        double change = 0.0;
        double change_percent = 0.0;

        status = umi_trading_workspace_visible_instrument_at(
            workspace, index, &market);
        if (status != UMI_STATUS_OK) break;
        if (market.has_bar) {
            change = market.bar.close - market.previous_close;
            if (market.previous_close != 0.0)
                change_percent = change / market.previous_close * 100.0;
        }
        (void)snprintf(key, sizeof(key), "trading.row.%zu", index);
        (void)snprintf(text, sizeof(text),
            "%s%s — %s — bid %.5f / ask %.5f — last %.5f — %+.2f%% — %s",
            strcmp(market.instrument.instrument_id.value,
                   snapshot.selected_instrument_id) == 0 ? "* " : "",
            market.instrument.symbol, market.instrument.venue,
            market.has_quote ? market.quote.bid : 0.0,
            market.has_quote ? market.quote.ask : 0.0,
            market.has_bar ? market.bar.close : 0.0, change_percent,
            umi_trading_market_state_text(market.market_state));
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.filter-instruments", "Filter…",
        "Filter the watchlist by symbol, venue or instrument identifier", 1);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.select-instrument", "Select…",
        "Select the active instrument for depth, charts and order entry",
        count > 0U);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.trading.refresh", "Refresh",
        "Refresh derived market and selection state", 1);
    return status;
}

UmiStatus umi_trading_ui_depth_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiTradingMarketSnapshot market;
    size_t count = 0U;
    size_t index;
    UmiStatus status = create_view(
        view_id, "trading-depth", "Market Depth",
        "Bid and ask ladders, spread, top liquidity and book imbalance.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.bid", snapshot.selected_bid);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.ask", snapshot.selected_ask);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.spread",
                            snapshot.selected_spread);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.depth-imbalance",
                            snapshot.selected_depth_imbalance);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.top-liquidity",
                            snapshot.selected_top_liquidity);
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument)
        status = umi_trading_workspace_selected_market(workspace, &market);
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument &&
        market.has_depth) {
        count = market.depth.bid_count > market.depth.ask_count
            ? market.depth.bid_count : market.depth.ask_count;
        count = visible_rows(count);
    }
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        char key[64];
        char text[320];
        double bid_price = index < market.depth.bid_count
            ? market.depth.bids[index].price : 0.0;
        double bid_size = index < market.depth.bid_count
            ? market.depth.bids[index].size : 0.0;
        double ask_price = index < market.depth.ask_count
            ? market.depth.asks[index].price : 0.0;
        double ask_size = index < market.depth.ask_count
            ? market.depth.asks[index].size : 0.0;

        (void)snprintf(key, sizeof(key), "trading.row.%zu", index);
        (void)snprintf(text, sizeof(text),
            "bid %.5f × %.2f  |  ask %.5f × %.2f",
            bid_price, bid_size, ask_price, ask_size);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.select-instrument", "Instrument…",
        "Choose the instrument whose order book is displayed",
        snapshot.visible_instrument_count > 0U);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.refresh", "Refresh",
        "Refresh the depth projection", 1);
    return status;
}

UmiStatus umi_trading_ui_chart_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiTradingMarketSnapshot market;
    UmiStatus status = create_view(
        view_id, "trading-chart", "Chart Analytics",
        "Selected-market OHLC evidence and the reusable chart workspace.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.pane-count",
                             (int64_t)snapshot.charts.panes_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.scale-count",
                             (int64_t)snapshot.charts.scales_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.annotation-count",
                             (int64_t)snapshot.charts.annotations_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.drawing-count",
                             (int64_t)snapshot.charts.drawings_count);
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument)
        status = umi_trading_workspace_selected_market(workspace, &market);
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument) {
        status = set_boolean(*out_view, "trading.has-bar", market.has_bar);
        if (status == UMI_STATUS_OK)
            status = set_number(*out_view, "trading.open",
                                market.has_bar ? market.bar.open : 0.0);
        if (status == UMI_STATUS_OK)
            status = set_number(*out_view, "trading.high",
                                market.has_bar ? market.bar.high : 0.0);
        if (status == UMI_STATUS_OK)
            status = set_number(*out_view, "trading.low",
                                market.has_bar ? market.bar.low : 0.0);
        if (status == UMI_STATUS_OK)
            status = set_number(*out_view, "trading.close",
                                market.has_bar ? market.bar.close : 0.0);
        if (status == UMI_STATUS_OK)
            status = set_number(*out_view, "trading.volume",
                                market.has_bar ? market.bar.volume : 0.0);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.select-instrument", "Instrument…",
        "Select the instrument projected into chart analytics",
        snapshot.visible_instrument_count > 0U);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.refresh", "Refresh",
        "Refresh chart and market evidence", 1);
    return status;
}

UmiStatus umi_trading_ui_order_ticket_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-order-ticket", "Order Ticket",
        "Simulation-first order drafting with visible pre-trade risk evidence.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "order.instrument",
                            snapshot.draft_order.instrument.symbol);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "order.side",
            umi_trading_side_text(snapshot.draft_order.side));
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "order.type",
            umi_trading_order_type_text(snapshot.draft_order.type));
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "order.time-in-force",
            umi_trading_time_in_force_text(snapshot.draft_order.tif));
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "order.quantity",
                            snapshot.draft_order.quantity);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "order.limit-price",
                            snapshot.draft_order.limit_price);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "order.stop-price",
                            snapshot.draft_order.stop_price);
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "risk.has-decision",
                             snapshot.has_draft_risk);
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "risk.allowed",
                             snapshot.draft_risk.allowed);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "risk.reason",
                            snapshot.draft_risk.reason);
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "order.can-submit",
                             snapshot.can_submit_order);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.set-side", "Side…",
        "Set the draft side to buy or sell", snapshot.has_selected_instrument);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.set-type", "Type / TIF…",
        "Set market, limit, stop or stop-limit and time in force",
        snapshot.has_selected_instrument);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.trading.set-quantity", "Quantity…",
        "Set a positive draft order quantity", snapshot.has_selected_instrument);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.trading.set-prices", "Prices…",
        "Set draft limit and stop prices", snapshot.has_selected_instrument);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 4U, "studio.action.trading.preview-order", "Preview Risk",
        "Run the pre-trade risk decision without submitting",
        snapshot.can_preview_order);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 5U, "studio.action.trading.submit-order", "Submit",
        "Submit only when environment and health safety gates permit",
        snapshot.can_submit_order);
    return status;
}

UmiStatus umi_trading_ui_orders_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    size_t count;
    size_t index;
    UmiStatus status = create_view(
        view_id, "trading-orders", "Orders",
        "Filter, inspect and cancel lifecycle-managed orders.", out_view);

    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "order.filter",
            umi_trading_workspace_order_filter_text(snapshot.order_filter));
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "order.visible-count",
                             (int64_t)snapshot.visible_order_count);
    count = visible_rows(snapshot.visible_order_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiOrder order;
        char key[64];
        char text[480];

        status = umi_trading_workspace_visible_order_at(workspace, index,
                                                         &order);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "trading.row.%zu", index);
        (void)snprintf(text, sizeof(text),
            "%s%s — %s %s %.4f %s @ %.5f — %s — filled %.4f @ %.5f",
            strcmp(order.request.client_order_id.value,
                   snapshot.selected_order_id) == 0 ? "* " : "",
            order.request.client_order_id.value,
            umi_trading_side_text(order.request.side),
            order.request.instrument.symbol, order.request.quantity,
            umi_trading_order_type_text(order.request.type),
            order.request.limit_price,
            umi_trading_order_status_text(order.status),
            order.filled_quantity, order.average_fill_price);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.filter-orders", "Filter…",
        "Show all, open, filled, cancelled or rejected orders", 1);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.select-order", "Select…",
        "Select an order by its client order identifier", count > 0U);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.trading.cancel-order", "Cancel Selected",
        "Cancel the selected non-terminal order", snapshot.can_cancel_order);
    return status;
}

UmiStatus umi_trading_ui_executions_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    size_t count;
    size_t index;
    UmiStatus status = create_view(
        view_id, "trading-executions", "Executions",
        "Newest-first immutable fill evidence for order reconciliation.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    count = visible_rows(snapshot.execution_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "execution.count",
                             (int64_t)snapshot.execution_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiExecutionReport report;
        char key[64];
        char text[420];

        status = umi_trading_workspace_execution_at(workspace, index, &report);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "trading.row.%zu", index);
        (void)snprintf(text, sizeof(text),
            "%s — order %s — %.4f @ %.5f — event %lld",
            report.execution_id.value, report.client_order_id.value,
            report.fill_quantity, report.fill_price,
            (long long)report.event_time_ms);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh",
        "Refresh execution evidence and position reconciliation", 1);
    return status;
}

UmiStatus umi_trading_ui_portfolio_risk_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    size_t count;
    size_t index;
    UmiStatus status = create_view(
        view_id, "trading-portfolio-risk", "Portfolio & Risk",
        "Positions, realised P&L, limits, environment gates and emergency stop.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "portfolio.gross-quantity",
                            snapshot.gross_position_quantity);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "portfolio.realised-pnl",
                            snapshot.realised_pnl);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "risk.max-order-quantity",
                            snapshot.risk_limit.max_order_quantity);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "risk.max-order-notional",
                            snapshot.risk_limit.max_order_notional);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "risk.max-position-quantity",
                            snapshot.risk_limit.max_position_quantity);
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "risk.max-daily-loss",
                            snapshot.risk_limit.max_daily_loss);
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "risk.kill-switch-reason",
                            snapshot.kill_switch_reason);
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "risk.live-armed",
                             snapshot.live_armed);
    count = visible_rows(snapshot.position_count);
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", (int64_t)count);
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiPosition position;
        char key[64];
        char text[360];

        status = umi_trading_workspace_position_at(workspace, index,
                                                    &position);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "trading.row.%zu", index);
        (void)snprintf(text, sizeof(text),
            "%s — quantity %.4f — average %.5f — realised P&L %.2f",
            position.instrument.symbol, position.quantity,
            position.average_price, position.realised_pnl);
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.set-environment", "Environment…",
        "Select simulation, paper or live environment", 1);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.engage-kill-switch", "Stop Trading",
        "Reject new orders immediately", !snapshot.kill_switch_engaged);
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.trading.reset-kill-switch", "Reset Stop",
        "Reset only after reviewing the recorded safety reason",
        snapshot.can_reset_kill_switch);
    return status;
}
