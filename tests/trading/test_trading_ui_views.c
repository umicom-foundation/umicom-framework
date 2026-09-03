/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_trading_ui_views.c
 *
 * PURPOSE:
 *   Verify all professional trading panes project one shared workspace and
 *   expose capability-aware Studio command actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "test_trading_common.h"
#include "umicom/trading_ui/trading_ui.h"
#include "umicom/ui/command_view.h"

typedef UmiStatus (*TradingViewFactory)(
    const char *, UmiTradingWorkspace *, UmiUiViewModel **);

/* Read one integer property and verify its type so a failed projection is
 * reported at the contract boundary rather than becoming a GUI-only fault. */
static int64_t integer_property(UmiUiViewModel *view, const char *key)
{
    UmiUiValue value;

    assert(umi_ui_view_model_get_property(view, key, &value) ==
           UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_INTEGER);
    return value.integer_value;
}

/*
 * Exercise verify view and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void verify_view(TradingViewFactory factory, const char *view_id,
                        const char *expected_kind,
                        UmiTradingWorkspace *workspace)
{
    UmiUiViewModel *view = NULL;
    UmiUiValue value;

    assert(factory(view_id, workspace, &view) == UMI_STATUS_OK);
    assert(umi_ui_view_model_get_property(
               view, "umicom.view-kind", &value) == UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(value.string_value, expected_kind) == 0);
    umi_ui_view_model_destroy(view);
}

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
    UmiTradeTick trade = {0};
    UmiTradingTradeTapeRecord trade_record;
    UmiUiViewModel *chart = NULL;
    UmiUiViewModel *tape = NULL;
    UmiUiViewModel *ticket = NULL;
    UmiUiCommandViewAction action;

    assert(umi_trading_workspace_create(NULL, &workspace) == UMI_STATUS_OK);
    assert(umi_trading_workspace_add_instrument(workspace, &instrument) ==
           UMI_STATUS_OK);
    quote.instrument = instrument;
    quote.bid = 24999.0;
    quote.ask = 25001.0;
    quote.bid_size = 3.0;
    quote.ask_size = 4.0;
    assert(umi_trading_workspace_update_quote(workspace, &quote) ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_set_health(workspace, 1, 0, 1) ==
           UMI_STATUS_OK);
    /* Seed the shared history before constructing the view so this test checks
     * the same retained chart state that a real presentation adapter receives. */
    bar.instrument = instrument;
    bar.start_time_ms = 1000U;
    bar.end_time_ms = 1999U;
    bar.open = 25000.0;
    bar.high = 25010.0;
    bar.low = 24990.0;
    bar.close = 25005.0;
    bar.volume = 12.0;
    /* Supply the preceding session close with the candle because the shared
     * workspace uses it to calculate change values for every trading view. */
    assert(umi_trading_workspace_update_bar(workspace, &bar, 24995.0) ==
           UMI_STATUS_OK);
    /* Public market trades use their own tape and are not inferred from the
     * quote, candle, or account execution histories. */
    trade.instrument = instrument;
    trade.price = 25005.0;
    trade.size = 8.0;
    trade.event_time_ms = 1999;
    assert(umi_trading_trade_tape_record_init(
               &trade_record, 1U, &trade,
               UMI_TRADING_TRADE_DIRECTION_BUYER_INITIATED, "regular") ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_update_trade(workspace, &trade_record) ==
           UMI_STATUS_OK);
    assert(umi_trading_workspace_set_chart_study(
               workspace, UMI_TRADING_CHART_STUDY_SIMPLE_AVERAGE, 12U) ==
           UMI_STATUS_OK);

    verify_view(umi_trading_ui_dashboard_view_create, "dashboard",
                "trading-dashboard", workspace);
    verify_view(umi_trading_ui_watchlist_view_create, "watchlist",
                "trading-watchlist", workspace);
    verify_view(umi_trading_ui_depth_view_create, "depth", "trading-depth",
                workspace);
    verify_view(umi_trading_ui_chart_view_create, "chart", "trading-chart",
                workspace);
    /* Inspect the chart directly as well as through the general factory check
     * to prove that adapter-neutral history and study properties are present. */
    assert(umi_trading_ui_chart_view_create(
               "chart.properties", workspace, &chart) == UMI_STATUS_OK);
    assert(integer_property(chart, "chart.bar-count") == 1);
    assert(integer_property(chart, "chart.study") ==
           UMI_TRADING_CHART_STUDY_SIMPLE_AVERAGE);
    assert(integer_property(chart, "chart.study-period") == 12);
    umi_ui_view_model_destroy(chart);
    verify_view(umi_trading_ui_order_ticket_view_create, "ticket",
                "trading-order-ticket", workspace);
    verify_view(umi_trading_ui_orders_view_create, "orders", "trading-orders",
                workspace);
    verify_view(umi_trading_ui_executions_view_create, "executions",
                "trading-executions", workspace);
    verify_view(umi_trading_ui_portfolio_risk_view_create, "portfolio",
                "trading-portfolio-risk", workspace);
    verify_view(umi_trading_ui_scanner_view_create, "scanner",
                "trading-scanner", workspace);
    verify_view(umi_trading_ui_predictive_lab_view_create, "predictive",
                "trading-predictive-lab", workspace);
    verify_view(umi_trading_ui_news_view_create, "news",
                "trading-news", workspace);
    verify_view(umi_trading_ui_context_inspector_view_create, "context",
                "trading-context-inspector", workspace);
    verify_view(umi_trading_ui_strategy_view_create, "strategy",
                "trading-strategy", workspace);
    verify_view(umi_trading_ui_replay_view_create, "replay",
                "trading-replay", workspace);
    verify_view(umi_trading_ui_research_output_view_create, "output",
                "trading-research-output", workspace);
    verify_view(umi_trading_ui_time_and_sales_view_create, "tape",
                "trading-time-and-sales", workspace);
    assert(umi_trading_ui_time_and_sales_view_create(
               "tape.properties", workspace, &tape) == UMI_STATUS_OK);
    assert(integer_property(tape, "trading.row-count") == 1);
    assert(umi_ui_command_view_action_at(tape, 0U, &action) == UMI_STATUS_OK);
    assert(strcmp(action.action_id,
                  UMI_TRADING_UI_ACTION_PAUSE_TRADE_TAPE) == 0);
    umi_ui_view_model_destroy(tape);
    verify_view(umi_trading_ui_economic_calendar_view_create, "events",
                "trading-economic-calendar", workspace);
    verify_view(umi_trading_ui_fundamentals_view_create, "fundamentals",
                "trading-fundamentals", workspace);
    verify_view(umi_trading_ui_alerts_view_create, "alerts",
                "trading-alerts", workspace);

    assert(umi_trading_ui_order_ticket_view_create(
               "ticket.actions", workspace, &ticket) == UMI_STATUS_OK);
    assert(umi_ui_command_view_action_at(ticket, 4U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id,
                  "studio.action.trading.preview-order") == 0);
    assert(action.enabled);
    assert(umi_ui_command_view_action_at(ticket, 5U, &action) ==
           UMI_STATUS_OK);
    assert(strcmp(action.action_id,
                  "studio.action.trading.submit-order") == 0);
    assert(action.enabled);
    umi_ui_view_model_destroy(ticket);

    umi_trading_workspace_destroy(workspace);
    return 0;
}
