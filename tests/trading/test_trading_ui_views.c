/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_trading_ui_views.c
 *
 * PURPOSE:
 *   Verify all professional trading panes project one shared workspace and
 *   expose capability-aware Studio command actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "test_trading_common.h"
#include "umicom/trading_ui/trading_ui.h"
#include "umicom/ui/command_view.h"

typedef UmiStatus (*TradingViewFactory)(
    const char *, UmiTradingWorkspace *, UmiUiViewModel **);

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

int main(void)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiInstrument instrument = test_instrument();
    UmiQuote quote = {0};
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

    verify_view(umi_trading_ui_dashboard_view_create, "dashboard",
                "trading-dashboard", workspace);
    verify_view(umi_trading_ui_watchlist_view_create, "watchlist",
                "trading-watchlist", workspace);
    verify_view(umi_trading_ui_depth_view_create, "depth", "trading-depth",
                workspace);
    verify_view(umi_trading_ui_chart_view_create, "chart", "trading-chart",
                workspace);
    verify_view(umi_trading_ui_order_ticket_view_create, "ticket",
                "trading-order-ticket", workspace);
    verify_view(umi_trading_ui_orders_view_create, "orders", "trading-orders",
                workspace);
    verify_view(umi_trading_ui_executions_view_create, "executions",
                "trading-executions", workspace);
    verify_view(umi_trading_ui_portfolio_risk_view_create, "portfolio",
                "trading-portfolio-risk", workspace);

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
