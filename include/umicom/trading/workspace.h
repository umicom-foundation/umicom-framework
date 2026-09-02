/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/workspace.h
 *
 * PURPOSE:
 *   Coordinate watchlists, market evidence, order entry, pre-trade risk,
 *   orders, executions, positions and chart infrastructure behind one stable,
 *   toolkit-neutral professional trading workspace contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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
#include "umicom/trading/alert.h"
#include "umicom/trading/execution_store.h"
#include "umicom/trading/oms.h"
#include "umicom/trading/position_book.h"
#include "umicom/trading/watchlist.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_TRADING_WORKSPACE_API_VERSION 2U
#define UMI_TRADING_WORKSPACE_FILTER_CAPACITY 96U

/**
 * List the named trading workspace order filter values accepted by this public contract.
 */
typedef enum UmiTradingWorkspaceOrderFilter {
    UMI_TRADING_WORKSPACE_ORDERS_ALL = 0,
    UMI_TRADING_WORKSPACE_ORDERS_OPEN = 1,
    UMI_TRADING_WORKSPACE_ORDERS_FILLED = 2,
    UMI_TRADING_WORKSPACE_ORDERS_CANCELLED = 3,
    UMI_TRADING_WORKSPACE_ORDERS_REJECTED = 4
} UmiTradingWorkspaceOrderFilter;

/**
 * Represent the trading market snapshot data shared with callers of this public contract.
 */
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

/**
 * Represent the trading workspace config data shared with callers of this public contract.
 */
typedef struct UmiTradingWorkspaceConfig {
    uint32_t structure_size;
    uint32_t api_version;
    UmiFinancialId account_id;
    UmiRiskLimit risk_limit;
    UmiTradingEnvironment environment;
} UmiTradingWorkspaceConfig;

/**
 * Represent the trading workspace snapshot data shared with callers of this public
 * contract.
 */
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
    size_t alert_count;
    size_t active_alert_count;
    size_t unacknowledged_alert_count;
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

/**
 * Represent the trading workspace data shared with callers of this public contract.
 */
typedef struct UmiTradingWorkspace UmiTradingWorkspace;

/**
 * Provide the trading workspace config default operation used by this module and its
 * client applications.
 */
UmiTradingWorkspaceConfig umi_trading_workspace_config_default(void);
/**
 * Initialise trading workspace from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_trading_workspace_create(
    const UmiTradingWorkspaceConfig *config,
    UmiTradingWorkspace **out_workspace);
/**
 * Release or reset state held by trading workspace so the same storage can be reused
 * safely.
 */
void umi_trading_workspace_destroy(UmiTradingWorkspace *workspace);

/**
 * Provide the trading workspace add instrument operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_add_instrument(
    UmiTradingWorkspace *workspace,
    const UmiInstrument *instrument);
/**
 * Provide the trading workspace remove instrument operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_remove_instrument(
    UmiTradingWorkspace *workspace,
    const char *instrument_id);
/**
 * Provide the trading workspace update quote operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_update_quote(
    UmiTradingWorkspace *workspace,
    const UmiQuote *quote);
/**
 * Provide the trading workspace update bar operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_update_bar(
    UmiTradingWorkspace *workspace,
    const UmiBar *bar,
    double previous_close);
/**
 * Provide the trading workspace update depth operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_update_depth(
    UmiTradingWorkspace *workspace,
    const UmiMarketDepth *depth);
/**
 * Provide the trading workspace set market state operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_market_state(
    UmiTradingWorkspace *workspace,
    const char *instrument_id,
    UmiMarketState state);

/**
 * Provide the trading workspace set instrument filter operation used by this module and
 * its client applications.
 */
UmiStatus umi_trading_workspace_set_instrument_filter(
    UmiTradingWorkspace *workspace,
    const char *filter_text);
/**
 * Provide the trading workspace set order filter operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_order_filter(
    UmiTradingWorkspace *workspace,
    UmiTradingWorkspaceOrderFilter order_filter);
/**
 * Provide the trading workspace select instrument operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_select_instrument(
    UmiTradingWorkspace *workspace,
    const char *instrument_id);
/**
 * Provide the trading workspace select order operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_select_order(
    UmiTradingWorkspace *workspace,
    const char *client_order_id);

/**
 * Provide the trading workspace set environment operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_environment(
    UmiTradingWorkspace *workspace,
    UmiTradingEnvironment environment);
/**
 * Provide the trading workspace set health operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_set_health(
    UmiTradingWorkspace *workspace,
    int market_data_ready,
    int broker_ready,
    int risk_ready);
/**
 * Provide the trading workspace set live armed operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_live_armed(
    UmiTradingWorkspace *workspace,
    int armed);

/**
 * Provide the trading workspace set draft side operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_draft_side(
    UmiTradingWorkspace *workspace,
    UmiSide side);
/**
 * Provide the trading workspace set draft type operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_draft_type(
    UmiTradingWorkspace *workspace,
    UmiOrderType type,
    UmiTimeInForce time_in_force);
/**
 * Provide the trading workspace set draft quantity operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_draft_quantity(
    UmiTradingWorkspace *workspace,
    double quantity);
/**
 * Provide the trading workspace set draft prices operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_draft_prices(
    UmiTradingWorkspace *workspace,
    double limit_price,
    double stop_price);
/**
 * Provide the trading workspace preview order operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_preview_order(
    UmiTradingWorkspace *workspace,
    UmiRiskDecision *out_decision);
/**
 * Provide the trading workspace submit order operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_submit_order(
    UmiTradingWorkspace *workspace,
    int64_t now_ms,
    UmiRiskDecision *out_decision);
/**
 * Provide the trading workspace cancel selected order operation used by this module and
 * its client applications.
 */
UmiStatus umi_trading_workspace_cancel_selected_order(
    UmiTradingWorkspace *workspace);
/**
 * Provide the trading workspace record execution operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_record_execution(
    UmiTradingWorkspace *workspace,
    const UmiExecutionReport *report);

/**
 * Provide the trading workspace engage kill switch operation used by this module and its
 * client applications.
 */
void umi_trading_workspace_engage_kill_switch(
    UmiTradingWorkspace *workspace,
    const char *reason);
/**
 * Provide the trading workspace reset kill switch operation used by this module and its
 * client applications.
 */
void umi_trading_workspace_reset_kill_switch(UmiTradingWorkspace *workspace);
/**
 * Provide the trading workspace refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_refresh(UmiTradingWorkspace *workspace);

/** Add a price alert for an instrument already known to the workspace. */
UmiStatus umi_trading_workspace_add_price_alert(
    UmiTradingWorkspace *workspace,
    const char *alert_id,
    const char *instrument_id,
    UmiTradingPriceAlertDirection direction,
    double threshold,
    int64_t created_at_ms);

/** Remove a price alert by stable identifier. */
UmiStatus umi_trading_workspace_remove_price_alert(
    UmiTradingWorkspace *workspace,
    const char *alert_id);

/** Enable or pause an existing price alert. */
UmiStatus umi_trading_workspace_set_price_alert_enabled(
    UmiTradingWorkspace *workspace,
    const char *alert_id,
    int enabled);

/** Acknowledge an active price alert without deleting its rule. */
UmiStatus umi_trading_workspace_acknowledge_price_alert(
    UmiTradingWorkspace *workspace,
    const char *alert_id);

/**
 * Provide the trading workspace snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_snapshot(
    UmiTradingWorkspace *workspace,
    UmiTradingWorkspaceSnapshot *out_snapshot);
/**
 * Find trading workspace visible instrument while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_trading_workspace_visible_instrument_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiTradingMarketSnapshot *out_market);
/**
 * Provide the trading workspace selected market operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_selected_market(
    UmiTradingWorkspace *workspace,
    UmiTradingMarketSnapshot *out_market);

/** Copy one price alert by position for presentation or persistence. */
UmiStatus umi_trading_workspace_price_alert_at(
    const UmiTradingWorkspace *workspace,
    size_t index,
    UmiTradingPriceAlert *out_alert);
/**
 * Find trading workspace visible order while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_trading_workspace_visible_order_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiOrder *out_order);
/**
 * Find trading workspace position while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_trading_workspace_position_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiPosition *out_position);
/**
 * Find trading workspace execution while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_trading_workspace_execution_at(
    UmiTradingWorkspace *workspace,
    size_t newest_first_index,
    UmiExecutionReport *out_report);

/**
 * Provide the trading workspace charts operation used by this module and its client
 * applications.
 */
UmiChartWorkspace *umi_trading_workspace_charts(
    UmiTradingWorkspace *workspace);
/**
 * Provide the trading environment text operation used by this module and its client
 * applications.
 */
const char *umi_trading_environment_text(UmiTradingEnvironment environment);
/**
 * Provide the trading side text operation used by this module and its client applications.
 */
const char *umi_trading_side_text(UmiSide side);
/**
 * Provide the trading order type text operation used by this module and its client
 * applications.
 */
const char *umi_trading_order_type_text(UmiOrderType type);
/**
 * Provide the trading time in force text operation used by this module and its client
 * applications.
 */
const char *umi_trading_time_in_force_text(UmiTimeInForce time_in_force);
/**
 * Provide the trading order status text operation used by this module and its client
 * applications.
 */
const char *umi_trading_order_status_text(UmiOrderStatus status);
/**
 * Provide the trading market state text operation used by this module and its client
 * applications.
 */
const char *umi_trading_market_state_text(UmiMarketState state);
/**
 * Provide the trading workspace order filter text operation used by this module and its
 * client applications.
 */
const char *umi_trading_workspace_order_filter_text(
    UmiTradingWorkspaceOrderFilter order_filter);

#ifdef __cplusplus
}
#endif

#endif
