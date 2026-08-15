/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/workspace.h
 *
 * PURPOSE:
 *   Coordinate watchlists, market evidence, order entry, pre-trade risk,
 *   orders, executions, positions and chart infrastructure behind one stable,
 *   toolkit-neutral professional trading workspace contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The workspace is deliberately safe by default. New instances start in the
 * simulation environment and never contact a broker. Paper and live order
 * submission remain disabled until a product reports broker readiness; live
 * submission additionally requires an explicit arming call. Studio can use
 * the complete workspace without gaining an accidental live-order path.
 */
#ifndef UMICOM_TRADING_WORKSPACE_H
#define UMICOM_TRADING_WORKSPACE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/chart/workspace.h"
#include "umicom/trading/execution_store.h"
#include "umicom/trading/oms.h"
#include "umicom/trading/position_book.h"
#include "umicom/trading/watchlist.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TRADING_WORKSPACE_API_VERSION 1U
#define UMI_TRADING_WORKSPACE_FILTER_CAPACITY 96U

typedef enum UmiTradingWorkspaceOrderFilter {
    UMI_TRADING_WORKSPACE_ORDERS_ALL = 0,
    UMI_TRADING_WORKSPACE_ORDERS_OPEN = 1,
    UMI_TRADING_WORKSPACE_ORDERS_FILLED = 2,
    UMI_TRADING_WORKSPACE_ORDERS_CANCELLED = 3,
    UMI_TRADING_WORKSPACE_ORDERS_REJECTED = 4
} UmiTradingWorkspaceOrderFilter;

typedef struct UmiTradingMarketSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    UmiInstrument instrument;
    UmiQuote quote;
    UmiBar bar;
    UmiMarketDepth depth;
    UmiMarketState market_state;
    double previous_close;
    int has_quote;
    int has_bar;
    int has_depth;
    uint64_t revision;
} UmiTradingMarketSnapshot;

typedef struct UmiTradingWorkspaceConfig {
    uint32_t structure_size;
    uint32_t api_version;
    UmiFinancialId account_id;
    UmiRiskLimit risk_limit;
    UmiTradingEnvironment environment;
} UmiTradingWorkspaceConfig;

typedef struct UmiTradingWorkspaceSnapshot {
    uint32_t structure_size;
    uint32_t api_version;
    UmiFinancialId account_id;
    UmiTradingEnvironment environment;
    UmiRiskLimit risk_limit;
    UmiOrderRequest draft_order;
    UmiRiskDecision draft_risk;
    UmiChartWorkspaceSnapshot charts;
    char instrument_filter[UMI_TRADING_WORKSPACE_FILTER_CAPACITY];
    UmiTradingWorkspaceOrderFilter order_filter;
    char selected_instrument_id[UMI_FINANCE_ID_CAPACITY];
    char selected_order_id[UMI_FINANCE_ID_CAPACITY];
    char kill_switch_reason[UMI_TRADING_TEXT_CAPACITY];
    size_t watchlist_count;
    size_t visible_instrument_count;
    size_t market_count;
    size_t order_count;
    size_t visible_order_count;
    size_t execution_count;
    size_t position_count;
    double gross_position_quantity;
    double realised_pnl;
    double selected_bid;
    double selected_ask;
    double selected_mid;
    double selected_spread;
    double selected_change;
    double selected_change_percent;
    double selected_depth_imbalance;
    double selected_top_liquidity;
    uint64_t revision;
    int market_data_ready;
    int broker_ready;
    int risk_ready;
    int health_ready;
    int live_armed;
    int kill_switch_engaged;
    int has_selected_instrument;
    int has_selected_order;
    int has_quote;
    int has_bar;
    int has_depth;
    int has_draft_risk;
    int can_preview_order;
    int can_submit_order;
    int can_cancel_order;
    int can_reset_kill_switch;
} UmiTradingWorkspaceSnapshot;

typedef struct UmiTradingWorkspace UmiTradingWorkspace;

UmiTradingWorkspaceConfig umi_trading_workspace_config_default(void);
UmiStatus umi_trading_workspace_create(
    const UmiTradingWorkspaceConfig *config,
    UmiTradingWorkspace **out_workspace);
void umi_trading_workspace_destroy(UmiTradingWorkspace *workspace);

UmiStatus umi_trading_workspace_add_instrument(
    UmiTradingWorkspace *workspace,
    const UmiInstrument *instrument);
UmiStatus umi_trading_workspace_remove_instrument(
    UmiTradingWorkspace *workspace,
    const char *instrument_id);
UmiStatus umi_trading_workspace_update_quote(
    UmiTradingWorkspace *workspace,
    const UmiQuote *quote);
UmiStatus umi_trading_workspace_update_bar(
    UmiTradingWorkspace *workspace,
    const UmiBar *bar,
    double previous_close);
UmiStatus umi_trading_workspace_update_depth(
    UmiTradingWorkspace *workspace,
    const UmiMarketDepth *depth);
UmiStatus umi_trading_workspace_set_market_state(
    UmiTradingWorkspace *workspace,
    const char *instrument_id,
    UmiMarketState state);

UmiStatus umi_trading_workspace_set_instrument_filter(
    UmiTradingWorkspace *workspace,
    const char *filter_text);
UmiStatus umi_trading_workspace_set_order_filter(
    UmiTradingWorkspace *workspace,
    UmiTradingWorkspaceOrderFilter order_filter);
UmiStatus umi_trading_workspace_select_instrument(
    UmiTradingWorkspace *workspace,
    const char *instrument_id);
UmiStatus umi_trading_workspace_select_order(
    UmiTradingWorkspace *workspace,
    const char *client_order_id);

UmiStatus umi_trading_workspace_set_environment(
    UmiTradingWorkspace *workspace,
    UmiTradingEnvironment environment);
UmiStatus umi_trading_workspace_set_health(
    UmiTradingWorkspace *workspace,
    int market_data_ready,
    int broker_ready,
    int risk_ready);
UmiStatus umi_trading_workspace_set_live_armed(
    UmiTradingWorkspace *workspace,
    int armed);

UmiStatus umi_trading_workspace_set_draft_side(
    UmiTradingWorkspace *workspace,
    UmiSide side);
UmiStatus umi_trading_workspace_set_draft_type(
    UmiTradingWorkspace *workspace,
    UmiOrderType type,
    UmiTimeInForce time_in_force);
UmiStatus umi_trading_workspace_set_draft_quantity(
    UmiTradingWorkspace *workspace,
    double quantity);
UmiStatus umi_trading_workspace_set_draft_prices(
    UmiTradingWorkspace *workspace,
    double limit_price,
    double stop_price);
UmiStatus umi_trading_workspace_preview_order(
    UmiTradingWorkspace *workspace,
    UmiRiskDecision *out_decision);
UmiStatus umi_trading_workspace_submit_order(
    UmiTradingWorkspace *workspace,
    int64_t now_ms,
    UmiRiskDecision *out_decision);
UmiStatus umi_trading_workspace_cancel_selected_order(
    UmiTradingWorkspace *workspace);
UmiStatus umi_trading_workspace_record_execution(
    UmiTradingWorkspace *workspace,
    const UmiExecutionReport *report);

void umi_trading_workspace_engage_kill_switch(
    UmiTradingWorkspace *workspace,
    const char *reason);
void umi_trading_workspace_reset_kill_switch(UmiTradingWorkspace *workspace);
UmiStatus umi_trading_workspace_refresh(UmiTradingWorkspace *workspace);

UmiStatus umi_trading_workspace_snapshot(
    UmiTradingWorkspace *workspace,
    UmiTradingWorkspaceSnapshot *out_snapshot);
UmiStatus umi_trading_workspace_visible_instrument_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiTradingMarketSnapshot *out_market);
UmiStatus umi_trading_workspace_selected_market(
    UmiTradingWorkspace *workspace,
    UmiTradingMarketSnapshot *out_market);
UmiStatus umi_trading_workspace_visible_order_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiOrder *out_order);
UmiStatus umi_trading_workspace_position_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiPosition *out_position);
UmiStatus umi_trading_workspace_execution_at(
    UmiTradingWorkspace *workspace,
    size_t newest_first_index,
    UmiExecutionReport *out_report);

UmiChartWorkspace *umi_trading_workspace_charts(
    UmiTradingWorkspace *workspace);
const char *umi_trading_environment_text(UmiTradingEnvironment environment);
const char *umi_trading_side_text(UmiSide side);
const char *umi_trading_order_type_text(UmiOrderType type);
const char *umi_trading_time_in_force_text(UmiTimeInForce time_in_force);
const char *umi_trading_order_status_text(UmiOrderStatus status);
const char *umi_trading_market_state_text(UmiMarketState state);
const char *umi_trading_workspace_order_filter_text(
    UmiTradingWorkspaceOrderFilter order_filter);

#ifdef __cplusplus
}
#endif

#endif
