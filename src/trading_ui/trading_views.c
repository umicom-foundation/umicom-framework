/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading_ui/trading_views.c
 *
 * PURPOSE:
 *   Project the safe professional trading workspace into reusable view models
 *   for dashboards, watchlists, depth, charts, order entry, order history,
 *   executions, portfolio positions and risk controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/trading_ui/trading_ui.h"

#include <stdint.h>
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

/* Provide the set string operation used by this module and its client applications. */
static UmiStatus set_string(UmiUiViewModel *view, const char *key,
                            const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(
        &value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set integer operation used by this module and its client applications. */
static UmiStatus set_integer(UmiUiViewModel *view, const char *key,
                             int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set number operation used by this module and its client applications. */
static UmiStatus set_number(UmiUiViewModel *view, const char *key,
                            double number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_real(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set boolean operation used by this module and its client applications. */
static UmiStatus set_boolean(UmiUiViewModel *view, const char *key,
                             int enabled)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, enabled);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value) : status;
}

/* Provide the set action operation used by this module and its client applications. */
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

/* Provide the create view operation used by this module and its client applications. */
static UmiStatus create_view(const char *view_id, const char *view_kind,
                             const char *title, const char *summary,
                             UmiUiViewModel **out_view)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (view_id == NULL || view_kind == NULL || out_view == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ui_view_model_create(view_id, "umicom.trading-ui",
                                      UMI_UI_ROLE_PANE, out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "umicom.view-kind", view_kind);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_string(*out_view, "title", title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "summary", summary);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

/*
 * Provide the set workspace properties operation used by this module and its client
 * applications.
 */
static UmiStatus set_workspace_properties(
    UmiUiViewModel *view, const UmiTradingWorkspaceSnapshot *snapshot)
{
    UmiStatus status = set_integer(
        view, "trading.revision", (int64_t)snapshot->revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(view, "trading.account-id",
                            snapshot->account_id.value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(view, "trading.environment",
            umi_trading_environment_text(snapshot->environment));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(view, "trading.selected-instrument",
                            snapshot->selected_instrument_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(view, "trading.selected-order",
                            snapshot->selected_order_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(view, "trading.market-data-ready",
                             snapshot->market_data_ready);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(view, "trading.broker-ready",
                             snapshot->broker_ready);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(view, "trading.risk-ready",
                             snapshot->risk_ready);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(view, "trading.health-ready",
                             snapshot->health_ready);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(view, "trading.kill-switch-engaged",
                             snapshot->kill_switch_engaged);
    return status;
}

/* Provide the take snapshot operation used by this module and its client applications. */
static UmiStatus take_snapshot(UmiTradingWorkspace *workspace,
                               UmiTradingWorkspaceSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return umi_trading_workspace_snapshot(workspace, out_snapshot);
}

/* Destroy a partially populated view when a later property or action fails. */
static UmiStatus finish_view(
    UmiStatus status,
    UmiUiViewModel **out_view)
{
    /* A failed factory must never transfer ownership of a half-built model. */
    if (status != UMI_STATUS_OK && out_view != NULL && *out_view != NULL) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

/*
 * Initialise trading ui dashboard view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_ui_dashboard_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-dashboard", "Trading Dashboard",
        "Environment safety, market health, activity and exposure at a glance.",
        out_view);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.instrument-count",
                             (int64_t)snapshot.watchlist_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.order-count",
                             (int64_t)snapshot.order_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.execution-count",
                             (int64_t)snapshot.execution_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.position-count",
                             (int64_t)snapshot.position_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.gross-position-quantity",
                            snapshot.gross_position_quantity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.realised-pnl",
                            snapshot.realised_pnl);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.selected-mid",
                            snapshot.selected_mid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.selected-change-percent",
                            snapshot.selected_change_percent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh",
        "Reconcile the trading workspace snapshot", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.set-environment", "Environment…",
        "Choose simulation, paper or live with explicit safety gates", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.trading.engage-kill-switch", "Stop Trading",
        "Engage the workspace kill switch immediately",
        !snapshot.kill_switch_engaged);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.trading.reset-kill-switch", "Reset Stop",
        "Reset the kill switch after the cause has been reviewed",
        snapshot.can_reset_kill_switch);
    return finish_view(status, out_view);
}

/*
 * Initialise trading ui watchlist view from caller-provided values so later operations
 * receive a known state.
 */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "trading.instrument-filter",
                            snapshot.instrument_filter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.visible-instrument-count",
                             (int64_t)snapshot.visible_instrument_count);
    count = visible_rows(snapshot.visible_instrument_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTradingMarketSnapshot market;
        char key[64];
        char text[480];
        double change = 0.0;
        double change_percent = 0.0;

        status = umi_trading_workspace_visible_instrument_at(
            workspace, index, &market);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        /* Apply this branch only when its contract condition is satisfied. */
        if (market.has_bar) {
            change = market.bar.close - market.previous_close;
            /* Apply this branch only when its contract condition is satisfied. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.filter-instruments", "Filter…",
        "Filter the watchlist by symbol, venue or instrument identifier", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.select-instrument", "Select…",
        "Select the active instrument for depth, charts and order entry",
        count > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.trading.refresh", "Refresh",
        "Refresh derived market and selection state", 1);
    return finish_view(status, out_view);
}

/* Project price-alert rules, activity and acknowledgement state. */
UmiStatus umi_trading_ui_alerts_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    size_t count;
    size_t index;
    UmiStatus status = create_view(
        view_id,
        "trading-alerts",
        "Price Alerts",
        "Price crossings for linked instruments with explicit acknowledgement.",
        out_view);

    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = take_snapshot(workspace, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_workspace_properties(*out_view, &snapshot);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view,
                             "alerts.rule-count",
                             (int64_t)snapshot.alert_count);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view,
                             "alerts.active-count",
                             (int64_t)snapshot.active_alert_count);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view,
                             "alerts.unacknowledged-count",
                             (int64_t)snapshot.unacknowledged_alert_count);
    }
    count = visible_rows(snapshot.alert_count);
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view,
                             "trading.row-count",
                             (int64_t)count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiTradingPriceAlert alert;
        char key[64];
        char text[480];
        const char *state_text;

        status = umi_trading_workspace_price_alert_at(
            workspace, index, &alert);
        if (status != UMI_STATUS_OK) {
            break;
        }
        /* Present active state first so urgent rules are clear in compact panels. */
        state_text = alert.active
            ? "ACTIVE"
            : (alert.enabled ? "watching" : "paused");
        (void)snprintf(key, sizeof(key), "trading.row.%zu", index);
        (void)snprintf(
            text,
            sizeof(text),
            "%s — %s %s %.5f — %s — %llu occurrence%s",
            alert.alert_id,
            alert.instrument_id,
            umi_trading_price_alert_direction_text(alert.direction),
            alert.threshold,
            state_text,
            (unsigned long long)alert.occurrences,
            alert.occurrences == 1U ? "" : "s");
        status = set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) {
        status = set_action(*out_view,
                            0U,
                            "studio.action.trading.refresh",
                            "Refresh",
                            "Refresh alert and linked market state",
                            1);
    }
    return finish_view(status, out_view);
}

/*
 * Initialise trading ui depth view from caller-provided values so later operations receive
 * a known state.
 */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.bid", snapshot.selected_bid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.ask", snapshot.selected_ask);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.spread",
                            snapshot.selected_spread);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.depth-imbalance",
                            snapshot.selected_depth_imbalance);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "trading.top-liquidity",
                            snapshot.selected_top_liquidity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument)
        status = umi_trading_workspace_selected_market(workspace, &market);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument &&
        market.has_depth) {
        count = market.depth.bid_count > market.depth.ask_count
            ? market.depth.bid_count : market.depth.ask_count;
        count = visible_rows(count);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.select-instrument", "Instrument…",
        "Choose the instrument whose order book is displayed",
        snapshot.visible_instrument_count > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.refresh", "Refresh",
        "Refresh the depth projection", 1);
    return finish_view(status, out_view);
}

/*
 * Initialise trading ui chart view from caller-provided values so later operations receive
 * a known state.
 */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.pane-count",
                             (int64_t)snapshot.charts.panes_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.scale-count",
                             (int64_t)snapshot.charts.scales_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.annotation-count",
                             (int64_t)snapshot.charts.annotations_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.drawing-count",
                             (int64_t)snapshot.charts.drawings_count);
    /* Publish the retained history size so every presentation adapter can
     * explain how much market evidence is currently available to the chart. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.bar-count",
                             (int64_t)snapshot.selected_bar_count);
    /* Keep the selected study in the shared view model instead of requiring
     * each desktop or web adapter to invent its own preference state. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.study",
                             (int64_t)snapshot.chart_study);
    /* Publish the calculation period beside the study because the two values
     * must travel together when a panel is detached or restored later. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "chart.study-period",
                             (int64_t)snapshot.chart_study_period);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument)
        status = umi_trading_workspace_selected_market(workspace, &market);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument) {
        status = set_boolean(*out_view, "trading.has-bar", market.has_bar);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            status = set_number(*out_view, "trading.open",
                                market.has_bar ? market.bar.open : 0.0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            status = set_number(*out_view, "trading.high",
                                market.has_bar ? market.bar.high : 0.0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            status = set_number(*out_view, "trading.low",
                                market.has_bar ? market.bar.low : 0.0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            status = set_number(*out_view, "trading.close",
                                market.has_bar ? market.bar.close : 0.0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK)
            status = set_number(*out_view, "trading.volume",
                                market.has_bar ? market.bar.volume : 0.0);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.select-instrument", "Instrument…",
        "Select the instrument projected into chart analytics",
        snapshot.visible_instrument_count > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.refresh", "Refresh",
        "Refresh chart and market evidence", 1);
    return status;
}

/*
 * Initialise trading ui order ticket view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_ui_order_ticket_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-order-ticket", "Order Ticket",
        "Simulation-first order drafting with visible pre-trade risk evidence.",
        out_view);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "order.instrument",
                            snapshot.draft_order.instrument.symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "order.side",
            umi_trading_side_text(snapshot.draft_order.side));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "order.type",
            umi_trading_order_type_text(snapshot.draft_order.type));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "order.time-in-force",
            umi_trading_time_in_force_text(snapshot.draft_order.tif));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "order.quantity",
                            snapshot.draft_order.quantity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "order.limit-price",
                            snapshot.draft_order.limit_price);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "order.stop-price",
                            snapshot.draft_order.stop_price);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "risk.has-decision",
                             snapshot.has_draft_risk);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "risk.allowed",
                             snapshot.draft_risk.allowed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "risk.reason",
                            snapshot.draft_risk.reason);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "order.can-submit",
                             snapshot.can_submit_order);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.set-side", "Side…",
        "Set the draft side to buy or sell", snapshot.has_selected_instrument);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.set-type", "Type / TIF…",
        "Set market, limit, stop or stop-limit and time in force",
        snapshot.has_selected_instrument);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.trading.set-quantity", "Quantity…",
        "Set a positive draft order quantity", snapshot.has_selected_instrument);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 3U, "studio.action.trading.set-prices", "Prices…",
        "Set draft limit and stop prices", snapshot.has_selected_instrument);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 4U, "studio.action.trading.preview-order", "Preview Risk",
        "Run the pre-trade risk decision without submitting",
        snapshot.can_preview_order);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 5U, "studio.action.trading.submit-order", "Submit",
        "Submit only when environment and health safety gates permit",
        snapshot.can_submit_order);
    return status;
}

/*
 * Initialise trading ui orders view from caller-provided values so later operations
 * receive a known state.
 */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "order.filter",
            umi_trading_workspace_order_filter_text(snapshot.order_filter));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "order.visible-count",
                             (int64_t)snapshot.visible_order_count);
    count = visible_rows(snapshot.visible_order_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiOrder order;
        char key[64];
        char text[480];

        status = umi_trading_workspace_visible_order_at(workspace, index,
                                                         &order);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.filter-orders", "Filter…",
        "Show all, open, filled, cancelled or rejected orders", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.select-order", "Select…",
        "Select an order by its client order identifier", count > 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.trading.cancel-order", "Cancel Selected",
        "Cancel the selected non-terminal order", snapshot.can_cancel_order);
    return status;
}

/*
 * Initialise trading ui executions view from caller-provided values so later operations
 * receive a known state.
 */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    count = visible_rows(snapshot.execution_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "execution.count",
                             (int64_t)snapshot.execution_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiExecutionReport report;
        char key[64];
        char text[420];

        status = umi_trading_workspace_execution_at(workspace, index, &report);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "trading.row.%zu", index);
        (void)snprintf(text, sizeof(text),
            "%s — order %s — %.4f @ %.5f — event %lld",
            report.execution_id.value, report.client_order_id.value,
            report.fill_quantity, report.fill_price,
            (long long)report.event_time_ms);
        status = set_string(*out_view, key, text);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh",
        "Refresh execution evidence and position reconciliation", 1);
    return status;
}

/*
 * Initialise trading ui portfolio risk view from caller-provided values so later
 * operations receive a known state.
 */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "portfolio.gross-quantity",
                            snapshot.gross_position_quantity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "portfolio.realised-pnl",
                            snapshot.realised_pnl);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "risk.max-order-quantity",
                            snapshot.risk_limit.max_order_quantity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "risk.max-order-notional",
                            snapshot.risk_limit.max_order_notional);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "risk.max-position-quantity",
                            snapshot.risk_limit.max_position_quantity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "risk.max-daily-loss",
                            snapshot.risk_limit.max_daily_loss);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "risk.kill-switch-reason",
                            snapshot.kill_switch_reason);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "risk.live-armed",
                             snapshot.live_armed);
    count = visible_rows(snapshot.position_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", (int64_t)count);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiPosition position;
        char key[64];
        char text[360];

        status = umi_trading_workspace_position_at(workspace, index,
                                                    &position);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "trading.row.%zu", index);
        (void)snprintf(text, sizeof(text),
            "%s — quantity %.4f — average %.5f — realised P&L %.2f",
            position.instrument.symbol, position.quantity,
            position.average_price, position.realised_pnl);
        status = set_string(*out_view, key, text);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.set-environment", "Environment…",
        "Select simulation, paper or live environment", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 1U, "studio.action.trading.engage-kill-switch", "Stop Trading",
        "Reject new orders immediately", !snapshot.kill_switch_engaged);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 2U, "studio.action.trading.reset-kill-switch", "Reset Stop",
        "Reset only after reviewing the recorded safety reason",
        snapshot.can_reset_kill_switch);
    return status;
}

/*
 * Initialise trading ui scanner view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_ui_scanner_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-scanner", "Market Scanner",
        "Cross-market instrument coverage, filters and data-quality state.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "scanner.filter",
                            snapshot.instrument_filter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "scanner.instrument-count",
                             (int64_t)snapshot.watchlist_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "scanner.visible-count",
                             (int64_t)snapshot.visible_instrument_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh",
        "Refresh the scanner from canonical market snapshots", 1);
    return status;
}

/*
 * Initialise trading ui predictive lab view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_trading_ui_predictive_lab_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-predictive-lab", "Predictive Research Lab",
        "Evidence-first change, liquidity and movement features for the selected instrument.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "predictive.change",
                            snapshot.selected_change);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "predictive.change-percent",
                            snapshot.selected_change_percent);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "predictive.depth-imbalance",
                            snapshot.selected_depth_imbalance);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_number(*out_view, "predictive.top-liquidity",
                            snapshot.selected_top_liquidity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "predictive.evidence-ready",
                             snapshot.has_bar && snapshot.has_quote);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh Evidence",
        "Refresh predictive evidence without issuing an order", 1);
    return status;
}

/*
 * Initialise trading ui news view from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_trading_ui_news_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-news", "Market News",
        "Instrument-linked news capability and provider readiness.", out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "news.provider-ready", 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "trading.row-count", 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "news.empty-state",
            "No accepted news provider is configured; trading remains available without news.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh",
        "Recheck the optional news-provider capability", 1);
    return status;
}

/*
 * Initialise trading ui context inspector view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_trading_ui_context_inspector_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-context-inspector", "Context Inspector",
        "Linked account, environment, instrument, order and safety context.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "context.has-instrument",
                             snapshot.has_selected_instrument);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "context.has-order",
                             snapshot.has_selected_order);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "context.workspace-revision",
                             (int64_t)snapshot.revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh Context",
        "Refresh linked trading context", 1);
    return status;
}

/*
 * Initialise trading ui strategy view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_ui_strategy_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-strategy", "Strategy",
        "Simulation-first strategy readiness, selection and safety evidence.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "strategy.simulation-mode",
                             snapshot.environment == UMI_TRADING_SIMULATION);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "strategy.order-route-disabled",
                             snapshot.environment != UMI_TRADING_LIVE ||
                             !snapshot.live_armed);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "strategy.selected-instrument",
                            snapshot.selected_instrument_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh Strategy",
        "Refresh strategy evidence without submitting orders", 1);
    return status;
}

/*
 * Initialise trading ui replay view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_trading_ui_replay_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-replay", "Market Replay",
        "Deterministic replay attachment, simulation safety and selected context.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "replay.attached", 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_boolean(*out_view, "replay.live-route-disabled", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_string(*out_view, "replay.empty-state",
            "No replay stream is attached; load accepted market evidence to begin.");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh Replay",
        "Refresh replay attachment state", 1);
    return status;
}

/*
 * Initialise trading ui research output view from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_trading_ui_research_output_view_create(
    const char *view_id, UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id, "trading-research-output", "Research Output",
        "Backtest, replay and evaluation evidence with workspace provenance.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_workspace_properties(*out_view, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "research.market-count",
                             (int64_t)snapshot.market_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "research.execution-count",
                             (int64_t)snapshot.execution_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "research.position-count",
                             (int64_t)snapshot.position_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = set_integer(*out_view, "research.workspace-revision",
                             (int64_t)snapshot.revision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = set_action(
        *out_view, 0U, "studio.action.trading.refresh", "Refresh Output",
        "Refresh research evidence and provenance", 1);
    return status;
}

/* Format retained public trades as normal row properties understood by every
 * existing view-model renderer. The source record remains strongly typed in
 * the workspace; only this presentation boundary creates readable text. */
static UmiStatus set_trade_tape_rows(
    UmiUiViewModel *view,
    UmiTradingWorkspace *workspace,
    size_t count)
{
    size_t row_count = visible_rows(count);
    size_t index;

    for (index = 0U; index < row_count; ++index) {
        UmiTradingTradeTapeRecord record;
        char key[48U];
        char text[256U];
        int written;
        UmiStatus status = umi_trading_workspace_selected_trade_at(
            workspace, index, &record);

        if (status != UMI_STATUS_OK) return status;
        written = snprintf(
            text,
            sizeof(text),
            "%.*s  %.8g x %.8g  %.*s  sequence %llu%.*s%.*s",
            (int)(UMI_TRADING_SYMBOL_CAPACITY - 1U),
            record.trade.instrument.symbol,
            record.trade.price,
            record.trade.size,
            24,
            umi_trading_trade_direction_text(record.direction),
            (unsigned long long)record.sequence,
            record.condition[0] != '\0' ? 3 : 0,
            " - ",
            (int)(UMI_TRADING_TRADE_CONDITION_CAPACITY - 1U),
            record.condition);
        if (written < 0 || (size_t)written >= sizeof(text)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        written = snprintf(key, sizeof(key), "trading.row.%zu", index);
        if (written < 0 || (size_t)written >= sizeof(key)) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        status = set_string(view, key, text);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/* Build a truthful trade-tape panel which never substitutes account fills for
 * public market trades when an accepted public feed is unavailable. */
UmiStatus umi_trading_ui_time_and_sales_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id,
        "trading-time-and-sales",
        "Time and Sales",
        "Sequence-aware public market trades for the linked instrument.",
        out_view);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_workspace_properties(*out_view, &snapshot);
    }
    if (status == UMI_STATUS_OK) {
        status = set_boolean(
            *out_view,
            "tape.provider-ready",
            snapshot.trade_tape.provider_ready);
    }
    if (status == UMI_STATUS_OK) {
        status = set_boolean(
            *out_view, "tape.paused", snapshot.trade_tape.paused);
    }
    if (status == UMI_STATUS_OK) {
        status = set_string(
            *out_view,
            "tape.filter",
            umi_trading_trade_tape_filter_text(snapshot.trade_tape.filter));
    }
    if (status == UMI_STATUS_OK) {
        status = set_number(
            *out_view, "tape.minimum-size", snapshot.trade_tape.minimum_size);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(
            *out_view,
            "tape.retained-count",
            (int64_t)snapshot.trade_tape.retained_count);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(
            *out_view,
            "tape.missing-sequence-count",
            snapshot.trade_tape.missing_sequence_count > (uint64_t)INT64_MAX
                ? INT64_MAX
                : (int64_t)snapshot.trade_tape.missing_sequence_count);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(
            *out_view,
            "tape.dropped-trade-count",
            snapshot.trade_tape.dropped_trade_count > (uint64_t)INT64_MAX
                ? INT64_MAX
                : (int64_t)snapshot.trade_tape.dropped_trade_count);
    }
    if (status == UMI_STATUS_OK) {
        status = set_integer(
            *out_view,
            "trading.row-count",
            (int64_t)snapshot.selected_trade_count);
    }
    if (status == UMI_STATUS_OK) {
        status = set_trade_tape_rows(
            *out_view, workspace, snapshot.selected_trade_count);
    }
    if (status == UMI_STATUS_OK && snapshot.selected_trade_count == 0U) {
        status = set_string(
            *out_view,
            "tape.empty-state",
            snapshot.trade_tape.provider_ready
                ? "No public trades match the selected instrument and filter."
                : "No accepted public trade feed is attached. Quotes and "
                  "account executions remain separate from market trades.");
    }
    if (status == UMI_STATUS_OK) {
        status = set_action(
            *out_view,
            0U,
            snapshot.trade_tape.paused
                ? UMI_TRADING_UI_ACTION_RESUME_TRADE_TAPE
                : UMI_TRADING_UI_ACTION_PAUSE_TRADE_TAPE,
            snapshot.trade_tape.paused ? "Resume Tape" : "Pause Tape",
            snapshot.trade_tape.paused
                ? "Show retained trades received while the display was paused"
                : "Freeze visible rows while trade ingestion continues",
            1);
    }
    if (status == UMI_STATUS_OK) {
        status = set_action(
            *out_view,
            1U,
            UMI_TRADING_UI_ACTION_REFRESH,
            "Refresh Tape",
            "Refresh Time and Sales from current Framework state",
            1);
    }
    return finish_view(status, out_view);
}

/* Build the reusable economic calendar capability projection. */
UmiStatus umi_trading_ui_economic_calendar_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiStatus status = create_view(
        view_id,
        "trading-economic-calendar",
        "Economic Calendar",
        "Provider-neutral events linked to the selected market context.",
        out_view);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_workspace_properties(*out_view, &snapshot);
    }
    /* Calendar data must come from an accepted adapter; an empty provider is a
     * normal capability state and does not affect safe trading operations. */
    if (status == UMI_STATUS_OK) {
        status = set_boolean(*out_view, "calendar.provider-ready", 0);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_integer(*out_view, "trading.row-count", 0);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_string(
            *out_view,
            "calendar.empty-state",
            "No accepted economic-event provider is configured.");
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_action(
            *out_view,
            0U,
            "studio.action.trading.refresh",
            "Refresh Events",
            "Recheck economic-event provider readiness",
            1);
    }
    return finish_view(status, out_view);
}

/* Build an instrument facts panel from data already owned by the workspace. */
UmiStatus umi_trading_ui_fundamentals_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view)
{
    UmiTradingWorkspaceSnapshot snapshot;
    UmiTradingMarketSnapshot market;
    UmiStatus status = create_view(
        view_id,
        "trading-fundamentals",
        "Fundamentals",
        "Instrument identity, venue and market evidence with optional research data.",
        out_view);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    status = take_snapshot(workspace, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_workspace_properties(*out_view, &snapshot);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument) {
        status = umi_trading_workspace_selected_market(workspace, &market);
    }
    /* A workspace with no selection is still a valid empty fundamentals view. */
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument) {
        status = set_string(
            *out_view, "fundamentals.symbol", market.instrument.symbol);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && snapshot.has_selected_instrument) {
        status = set_string(
            *out_view, "fundamentals.venue", market.instrument.venue);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_number(
            *out_view, "fundamentals.market-change-percent",
            snapshot.selected_change_percent);
    }
    /* Company accounts and classifications require a separate accepted data
     * adapter, so readiness is explicit while market identity remains useful. */
    if (status == UMI_STATUS_OK) {
        status = set_boolean(*out_view, "fundamentals.provider-ready", 0);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = set_action(
            *out_view,
            0U,
            "studio.action.trading.refresh",
            "Refresh Research",
            "Refresh instrument facts and optional research readiness",
            1);
    }
    return finish_view(status, out_view);
}
