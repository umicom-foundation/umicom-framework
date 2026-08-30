/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading_ui/trading_ui.h
 *
 * PURPOSE:
 *   Declare toolkit-neutral view factories and interaction contracts for the
 *   professional trading workspace. Every frontend receives the same market,
 *   order, execution, portfolio and risk projection and routes mutations
 *   through the same guarded Framework controller.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
#include "umicom/trading_ui/actions.h"
#include "umicom/trading_ui/action_controller.h"
#include "umicom/trading_ui/simulation_market.h"

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
#define UMI_TRADING_UI_VIEW_SCANNER "umicom.trading.workspace.scanner"
#define UMI_TRADING_UI_VIEW_PREDICTIVE_LAB \
    "umicom.trading.workspace.predictive-lab"
#define UMI_TRADING_UI_VIEW_NEWS "umicom.trading.workspace.news"
#define UMI_TRADING_UI_VIEW_CONTEXT_INSPECTOR \
    "umicom.trading.workspace.context-inspector"
#define UMI_TRADING_UI_VIEW_STRATEGY "umicom.trading.workspace.strategy"
#define UMI_TRADING_UI_VIEW_REPLAY "umicom.trading.workspace.replay"
#define UMI_TRADING_UI_VIEW_RESEARCH_OUTPUT \
    "umicom.trading.workspace.research-output"

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
UmiStatus umi_trading_ui_scanner_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_predictive_lab_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_news_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_context_inspector_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_strategy_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_replay_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);
UmiStatus umi_trading_ui_research_output_view_create(
    const char *view_id,
    UmiTradingWorkspace *workspace,
    UmiUiViewModel **out_view);

#ifdef __cplusplus
}
#endif

#endif
