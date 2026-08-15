/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/trading_ui.h
 *
 * PURPOSE:
 *   Declare toolkit-neutral view factories for the professional trading
 *   workspace. Every frontend receives the same market, order, execution,
 *   portfolio and risk projection without depending on GTK, Qt or Wt.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A factory takes the shared trading workspace and returns a generic Umicom
 * view model. Applications decide where that model appears; the Framework
 * decides what professional trading information and actions it exposes.
 */
#ifndef UMICOM_TRADING_UI_TRADING_UI_H
#define UMICOM_TRADING_UI_TRADING_UI_H

#include "umicom/trading/workspace.h"
#include "umicom/ui/view_model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TRADING_UI_VISIBLE_ROWS 24U
#define UMI_TRADING_UI_VIEW_DASHBOARD "umicom.trading.workspace.dashboard"
#define UMI_TRADING_UI_VIEW_WATCHLIST "umicom.trading.workspace.watchlist"
#define UMI_TRADING_UI_VIEW_DEPTH "umicom.trading.workspace.depth"
#define UMI_TRADING_UI_VIEW_CHART "umicom.trading.workspace.chart"
#define UMI_TRADING_UI_VIEW_ORDER_TICKET "umicom.trading.workspace.order-ticket"
#define UMI_TRADING_UI_VIEW_ORDERS "umicom.trading.workspace.orders"
#define UMI_TRADING_UI_VIEW_EXECUTIONS "umicom.trading.workspace.executions"
#define UMI_TRADING_UI_VIEW_PORTFOLIO_RISK \
    "umicom.trading.workspace.portfolio-risk"

UmiStatus umi_trading_ui_dashboard_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_watchlist_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_depth_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_chart_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_order_ticket_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_orders_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_executions_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_portfolio_risk_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
