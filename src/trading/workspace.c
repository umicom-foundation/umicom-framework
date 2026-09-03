/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/workspace.c
 *
 * PURPOSE:
 *   Implement the professional trading workspace by composing the existing
 *   Framework watchlist, OMS, risk, execution, position and chart services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source does not implement a broker adapter. It coordinates canonical
 * records and the reference OMS, making the same state available to desktop,
 * web, headless and automation frontends. Live execution remains gated by
 * broker readiness and an explicit arming call performed by a trading product.
 */
#include "umicom/trading/workspace.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/finance/identifier.h"
#include "umicom/trading/bar.h"
#include "umicom/trading/depth.h"
#include "umicom/trading/environment.h"
#include "umicom/trading/execution_report.h"
#include "umicom/trading/health.h"
#include "umicom/trading/instrument.h"
#include "umicom/trading/market_state.h"
#include "umicom/trading/order_book.h"
#include "umicom/trading/order_request.h"
#include "umicom/trading/order_type.h"
#include "umicom/trading/pnl.h"
#include "umicom/trading/portfolio.h"
#include "umicom/trading/position.h"
#include "umicom/trading/pretrade_risk.h"
#include "umicom/trading/quote.h"
#include "umicom/trading/risk_decision.h"
#include "umicom/trading/risk_limit.h"
#include "umicom/trading/time_in_force.h"

/* Retain a bounded chronological candle series beside each market snapshot.
 * Keeping this private avoids copying all chart history with every quote row. */
typedef struct UmiTradingBarHistory {
    UmiBar bars[UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY];
    size_t count;
} UmiTradingBarHistory;

struct UmiTradingWorkspace {
    UmiFinancialId account_id;
    UmiTradingEnvironment environment;
    UmiWatchlist watchlist;
    UmiTradingMarketSnapshot markets[UMI_TRADING_MAX_WATCHLIST];
    UmiTradingBarHistory bar_histories[UMI_TRADING_MAX_WATCHLIST];
    size_t market_count;
    UmiOms oms;
    UmiExecutionStore executions;
    UmiPositionBook positions;
    UmiTradingAlertBook alerts;
    UmiChartWorkspace *charts;
    UmiOrderRequest draft_order;
    UmiRiskDecision draft_risk;
    char instrument_filter[UMI_TRADING_WORKSPACE_FILTER_CAPACITY];
    UmiTradingWorkspaceOrderFilter order_filter;
    UmiTradingChartStudy chart_study;
    size_t chart_study_period;
    char selected_instrument_id[UMI_FINANCE_ID_CAPACITY];
    char selected_order_id[UMI_FINANCE_ID_CAPACITY];
    uint64_t next_order_sequence;
    uint64_t revision;
    int market_data_ready;
    int broker_ready;
    int risk_ready;
    int live_armed;
    int has_draft_risk;
};

/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) source = "";
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

/* Provide the valid environment operation used by this module and its client applications. */
static int valid_environment(UmiTradingEnvironment environment)
{
    return environment >= UMI_TRADING_SIMULATION &&
           environment <= UMI_TRADING_LIVE;
}

/*
 * Provide the valid order filter operation used by this module and its client
 * applications.
 */
static int valid_order_filter(UmiTradingWorkspaceOrderFilter order_filter)
{
    return order_filter >= UMI_TRADING_WORKSPACE_ORDERS_ALL &&
           order_filter <= UMI_TRADING_WORKSPACE_ORDERS_REJECTED;
}

/*
 * Provide the contains case insensitive operation used by this module and its client
 * applications.
 */
static int contains_case_insensitive(const char *text, const char *query)
{
    const unsigned char *candidate;
    size_t query_length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (query == NULL || query[0] == '\0') return 1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return 0;
    query_length = strlen(query);
    /* Visit each bounded item once so every record receives the same rule. */
    for (candidate = (const unsigned char *)text;
         *candidate != '\0'; ++candidate) {
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < query_length; ++index) {
            unsigned char left = candidate[index];
            unsigned char right = (unsigned char)query[index];
            /* Apply this branch only when its contract condition is satisfied. */
            if (left == '\0' || tolower(left) != tolower(right)) break;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index == query_length) return 1;
    }
    return 0;
}

/* Provide the market index operation used by this module and its client applications. */
static size_t market_index(const UmiTradingWorkspace *workspace,
                           const char *instrument_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || instrument_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workspace->market_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(workspace->markets[index].instrument.instrument_id.value,
                   instrument_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the order index operation used by this module and its client applications. */
static size_t order_index(const UmiTradingWorkspace *workspace,
                          const char *client_order_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || client_order_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workspace->oms.orders.count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(workspace->oms.orders.orders[index]
                       .request.client_order_id.value,
                   client_order_id) == 0) return index;
    }
    return SIZE_MAX;
}

/* Provide the market visible operation used by this module and its client applications. */
static int market_visible(const UmiTradingWorkspace *workspace,
                          const UmiTradingMarketSnapshot *market)
{
    return contains_case_insensitive(market->instrument.symbol,
                                     workspace->instrument_filter) ||
           contains_case_insensitive(market->instrument.venue,
                                     workspace->instrument_filter) ||
           contains_case_insensitive(market->instrument.instrument_id.value,
                                     workspace->instrument_filter) ||
           contains_case_insensitive(market->instrument.currency.code,
                                     workspace->instrument_filter);
}

/* Provide the order visible operation used by this module and its client applications. */
static int order_visible(const UmiTradingWorkspace *workspace,
                         const UmiOrder *order)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (workspace->order_filter) {
        case UMI_TRADING_WORKSPACE_ORDERS_OPEN:
            return order->status == UMI_ORDER_NEW ||
                   order->status == UMI_ORDER_VALIDATED ||
                   order->status == UMI_ORDER_ACCEPTED ||
                   order->status == UMI_ORDER_PARTIALLY_FILLED;
        case UMI_TRADING_WORKSPACE_ORDERS_FILLED:
            return order->status == UMI_ORDER_FILLED;
        case UMI_TRADING_WORKSPACE_ORDERS_CANCELLED:
            return order->status == UMI_ORDER_CANCELLED;
        case UMI_TRADING_WORKSPACE_ORDERS_REJECTED:
            return order->status == UMI_ORDER_REJECTED;
        case UMI_TRADING_WORKSPACE_ORDERS_ALL:
        default:
            return 1;
    }
}

/* Return the number of records represented by visible market without changing their state. */
static size_t visible_market_count(const UmiTradingWorkspace *workspace)
{
    size_t index;
    size_t count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workspace->market_count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (market_visible(workspace, &workspace->markets[index])) count += 1U;
    return count;
}

/* Return the number of records represented by visible order without changing their state. */
static size_t visible_order_count(const UmiTradingWorkspace *workspace)
{
    size_t index;
    size_t count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workspace->oms.orders.count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (order_visible(workspace, &workspace->oms.orders.orders[index]))
            count += 1U;
    return count;
}

/*
 * Provide the current position quantity operation used by this module and its client
 * applications.
 */
static double current_position_quantity(UmiTradingWorkspace *workspace)
{
    UmiPosition *position = NULL;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (workspace->draft_order.instrument.instrument_id.value[0] == '\0')
        return 0.0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_position_book_get(&workspace->positions,
                              &workspace->draft_order.instrument, 0,
                              &position) != UMI_STATUS_OK) return 0.0;
    return position->quantity;
}

/* Provide the realised pnl operation used by this module and its client applications. */
static double realised_pnl(const UmiTradingWorkspace *workspace)
{
    size_t index;
    double total = 0.0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workspace->positions.count; ++index)
        total += workspace->positions.positions[index].realised_pnl;
    return total;
}

/* Provide the initialise draft operation used by this module and its client applications. */
static void initialise_draft(UmiTradingWorkspace *workspace)
{
    memset(&workspace->draft_order, 0, sizeof(workspace->draft_order));
    copy_text(workspace->draft_order.client_order_id.value,
              sizeof(workspace->draft_order.client_order_id.value),
              "draft-order");
    workspace->draft_order.account_id = workspace->account_id;
    workspace->draft_order.side = UMI_SIDE_BUY;
    workspace->draft_order.type = UMI_ORDER_LIMIT;
    workspace->draft_order.tif = UMI_TIF_DAY;
    workspace->draft_order.quantity = 1.0;
    workspace->draft_order.environment = workspace->environment;
    workspace->has_draft_risk = 0;
}

/* Provide the choose instrument operation used by this module and its client applications. */
static void choose_instrument(UmiTradingWorkspace *workspace,
                              const UmiTradingMarketSnapshot *market)
{
    double reference_price = 0.0;
    copy_text(workspace->selected_instrument_id,
              sizeof(workspace->selected_instrument_id),
              market->instrument.instrument_id.value);
    workspace->draft_order.instrument = market->instrument;
    /* Apply this branch only when its contract condition is satisfied. */
    if (market->has_quote) reference_price = umi_quote_mid(&market->quote);
    else /* Apply this branch only when its contract condition is satisfied. */ if (market->has_bar) reference_price = market->bar.close;
    workspace->draft_order.limit_price = reference_price;
    workspace->draft_order.stop_price = 0.0;
    workspace->has_draft_risk = 0;
}

/*
 * Provide the reconcile selections operation used by this module and its client
 * applications.
 */
static void reconcile_selections(UmiTradingWorkspace *workspace)
{
    size_t index = market_index(workspace, workspace->selected_instrument_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX || !market_visible(workspace,
                                             &workspace->markets[index])) {
        workspace->selected_instrument_id[0] = '\0';
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < workspace->market_count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (market_visible(workspace, &workspace->markets[index])) {
                choose_instrument(workspace, &workspace->markets[index]);
                break;
            }
        }
    }

    index = order_index(workspace, workspace->selected_order_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX ||
        !order_visible(workspace, &workspace->oms.orders.orders[index])) {
        workspace->selected_order_id[0] = '\0';
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = workspace->oms.orders.count; index > 0U; --index) {
            UmiOrder *order = &workspace->oms.orders.orders[index - 1U];
            /* Apply this operation only while the related capability or state is available. */
            if (order_visible(workspace, order)) {
                copy_text(workspace->selected_order_id,
                          sizeof(workspace->selected_order_id),
                          order->request.client_order_id.value);
                break;
            }
        }
    }
}

/*
 * Provide the trading workspace config default operation used by this module and its
 * client applications.
 */
UmiTradingWorkspaceConfig umi_trading_workspace_config_default(void)
{
    UmiTradingWorkspaceConfig config;
    memset(&config, 0, sizeof(config));
    config.structure_size = (uint32_t)sizeof(config);
    config.api_version = UMI_TRADING_WORKSPACE_API_VERSION;
    copy_text(config.account_id.value, sizeof(config.account_id.value),
              "simulation.account");
    config.risk_limit.max_order_quantity = 1000.0;
    config.risk_limit.max_order_notional = 10000000.0;
    config.risk_limit.max_position_quantity = 5000.0;
    config.risk_limit.max_daily_loss = 100000.0;
    config.environment = UMI_TRADING_SIMULATION;
    return config;
}

/*
 * Initialise trading workspace from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_trading_workspace_create(
    const UmiTradingWorkspaceConfig *config,
    UmiTradingWorkspace **out_workspace)
{
    UmiTradingWorkspaceConfig effective;
    UmiTradingWorkspace *workspace;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_workspace = NULL;
    effective = config != NULL
        ? *config : umi_trading_workspace_config_default();
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective.structure_size < sizeof(effective) ||
        effective.api_version != UMI_TRADING_WORKSPACE_API_VERSION ||
        !umi_financial_id_valid(&effective.account_id) ||
        !umi_risk_limit_valid(&effective.risk_limit) ||
        !valid_environment(effective.environment)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    workspace = (UmiTradingWorkspace *)calloc(1U, sizeof(*workspace));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workspace->account_id = effective.account_id;
    workspace->environment = effective.environment;
    workspace->order_filter = UMI_TRADING_WORKSPACE_ORDERS_ALL;
    workspace->next_order_sequence = 1U;
    workspace->revision = 1U;
    workspace->risk_ready = 1;
    umi_watchlist_init(&workspace->watchlist);
    umi_oms_init(&workspace->oms, effective.risk_limit);
    umi_execution_store_init(&workspace->executions);
    umi_position_book_init(&workspace->positions);
    umi_trading_alert_book_init(&workspace->alerts);
    initialise_draft(workspace);
    /* A neutral candle view is the least surprising default; the period is
     * retained now so selecting a study later does not require another choice. */
    workspace->chart_study = UMI_TRADING_CHART_STUDY_NONE;
    workspace->chart_study_period = 20U;
    status = umi_chart_workspace_create(&workspace->charts);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_trading_workspace_destroy(workspace);
        return status;
    }
    *out_workspace = workspace;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by trading workspace so the same storage can be reused
 * safely.
 */
void umi_trading_workspace_destroy(UmiTradingWorkspace *workspace)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return;
    umi_chart_workspace_destroy(workspace->charts);
    free(workspace);
}

/*
 * Provide the trading workspace add instrument operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_add_instrument(
    UmiTradingWorkspace *workspace,
    const UmiInstrument *instrument)
{
    UmiTradingMarketSnapshot *market;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || instrument == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (workspace->market_count >= UMI_TRADING_MAX_WATCHLIST)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_watchlist_add(&workspace->watchlist, instrument);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    market = &workspace->markets[workspace->market_count];
    memset(market, 0, sizeof(*market));
    /* A removed market can leave bytes beyond the active count, so reset the
     * matching history slot before it is assigned to another instrument. */
    memset(&workspace->bar_histories[workspace->market_count],
           0,
           sizeof(workspace->bar_histories[workspace->market_count]));
    workspace->market_count += 1U;
    market->structure_size = (uint32_t)sizeof(*market);
    market->api_version = UMI_TRADING_WORKSPACE_API_VERSION;
    market->instrument = *instrument;
    market->market_state = UMI_MARKET_CLOSED;
    market->revision = 1U;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (workspace->selected_instrument_id[0] == '\0')
        choose_instrument(workspace, market);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace remove instrument operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_remove_instrument(
    UmiTradingWorkspace *workspace,
    const char *instrument_id)
{
    size_t index;
    size_t alert_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || instrument_id == NULL || instrument_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, instrument_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /*
     * Remove dependent alerts first so the workspace cannot retain rules for
     * an instrument that is no longer available. Reverse iteration remains
     * correct while each successful removal compacts the alert array.
     */
    for (alert_index = workspace->alerts.count;
         alert_index > 0U;
         --alert_index) {
        UmiTradingPriceAlert *alert =
            &workspace->alerts.alerts[alert_index - 1U];

        if (strcmp(alert->instrument_id, instrument_id) == 0) {
            (void)umi_trading_alert_book_remove(&workspace->alerts,
                                                alert->alert_id);
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < workspace->market_count) {
        memmove(&workspace->markets[index], &workspace->markets[index + 1U],
                (workspace->market_count - index - 1U) *
                    sizeof(workspace->markets[0]));
        memmove(&workspace->bar_histories[index],
                &workspace->bar_histories[index + 1U],
                (workspace->market_count - index - 1U) *
                    sizeof(workspace->bar_histories[0]));
        memmove(&workspace->watchlist.instruments[index],
                &workspace->watchlist.instruments[index + 1U],
                (workspace->watchlist.count - index - 1U) *
                    sizeof(workspace->watchlist.instruments[0]));
    }
    workspace->market_count -= 1U;
    workspace->watchlist.count -= 1U;
    /* Clear the inactive tail so removed market data cannot be observed if the
     * slot is reused by a later instrument. */
    memset(&workspace->bar_histories[workspace->market_count],
           0,
           sizeof(workspace->bar_histories[workspace->market_count]));
    workspace->revision += 1U;
    reconcile_selections(workspace);
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace update quote operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_update_quote(
    UmiTradingWorkspace *workspace,
    const UmiQuote *quote)
{
    size_t index;
    UmiStatus alert_status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || quote == NULL || !umi_quote_valid(quote))
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, quote->instrument.instrument_id.value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /*
     * Alerts observe the neutral quote midpoint. A legacy provider may omit a
     * timestamp, so zero is used instead of rejecting otherwise valid prices.
     */
    alert_status = umi_trading_alert_book_evaluate(
        &workspace->alerts,
        quote->instrument.instrument_id.value,
        umi_quote_mid(quote),
        quote->event_time_ms >= 0 ? quote->event_time_ms : 0);
    if (alert_status != UMI_STATUS_OK) {
        return alert_status;
    }
    /* Commit the quote only after every dependent alert accepted the value. */
    workspace->markets[index].quote = *quote;
    workspace->markets[index].has_quote = 1;
    workspace->markets[index].revision += 1U;
    workspace->market_data_ready = 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(workspace->selected_instrument_id,
               quote->instrument.instrument_id.value) == 0 &&
        workspace->draft_order.limit_price <= 0.0) {
        workspace->draft_order.limit_price = umi_quote_mid(quote);
    }
    workspace->revision += 1U;
    workspace->has_draft_risk = 0;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace update bar operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_update_bar(
    UmiTradingWorkspace *workspace,
    const UmiBar *bar,
    double previous_close)
{
    size_t index;
    UmiTradingBarHistory *history;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || bar == NULL || !umi_bar_valid(bar) ||
        previous_close < 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, bar->instrument.instrument_id.value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    history = &workspace->bar_histories[index];
    /* A repeated start time updates the still-forming candle. Older completed
     * candles are rejected because silently reordering them would make live
     * studies and visual history disagree. */
    if (history->count > 0U &&
        bar->start_time_ms < history->bars[history->count - 1U].start_time_ms) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (history->count > 0U &&
        bar->start_time_ms == history->bars[history->count - 1U].start_time_ms) {
        history->bars[history->count - 1U] = *bar;
    } else {
        /* Once capacity is reached, discard only the oldest candle and keep
         * the most recent fixed-size window needed by interactive charts. */
        if (history->count == UMI_TRADING_WORKSPACE_BAR_HISTORY_CAPACITY) {
            memmove(&history->bars[0],
                    &history->bars[1],
                    (history->count - 1U) * sizeof(history->bars[0]));
            history->count -= 1U;
        }
        history->bars[history->count++] = *bar;
    }
    workspace->markets[index].bar = *bar;
    workspace->markets[index].previous_close = previous_close;
    workspace->markets[index].has_bar = 1;
    workspace->markets[index].revision += 1U;
    workspace->market_data_ready = 1;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace update depth operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_update_depth(
    UmiTradingWorkspace *workspace,
    const UmiMarketDepth *depth)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || depth == NULL || !umi_market_depth_valid(depth))
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, depth->instrument.instrument_id.value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    workspace->markets[index].depth = *depth;
    workspace->markets[index].has_depth = 1;
    workspace->markets[index].revision += 1U;
    workspace->market_data_ready = 1;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace set market state operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_market_state(
    UmiTradingWorkspace *workspace,
    const char *instrument_id,
    UmiMarketState state)
{
    size_t index;
    UmiTradingMarketSnapshot *market;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || instrument_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, instrument_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    market = &workspace->markets[index];
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_market_state_transition_allowed(market->market_state, state))
        return UMI_STATUS_INVALID_STATE;
    market->market_state = state;
    market->revision += 1U;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace set instrument filter operation used by this module and
 * its client applications.
 */
UmiStatus umi_trading_workspace_set_instrument_filter(
    UmiTradingWorkspace *workspace,
    const char *filter_text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    copy_text(workspace->instrument_filter,
              sizeof(workspace->instrument_filter), filter_text);
    workspace->revision += 1U;
    reconcile_selections(workspace);
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace set order filter operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_order_filter(
    UmiTradingWorkspace *workspace,
    UmiTradingWorkspaceOrderFilter order_filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || !valid_order_filter(order_filter))
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->order_filter = order_filter;
    workspace->revision += 1U;
    reconcile_selections(workspace);
    return UMI_STATUS_OK;
}

/* Persist a bounded study choice in toolkit-neutral workspace state. */
UmiStatus umi_trading_workspace_set_chart_study(
    UmiTradingWorkspace *workspace,
    UmiTradingChartStudy study,
    size_t period)
{
    if (workspace == NULL ||
        (study != UMI_TRADING_CHART_STUDY_NONE &&
         study != UMI_TRADING_CHART_STUDY_SIMPLE_AVERAGE &&
         study != UMI_TRADING_CHART_STUDY_EXPONENTIAL_AVERAGE) ||
        period < 2U || period > 200U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (workspace->chart_study == study &&
        workspace->chart_study_period == period) {
        return UMI_STATUS_OK;
    }
    workspace->chart_study = study;
    workspace->chart_study_period = period;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace select instrument operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_select_instrument(
    UmiTradingWorkspace *workspace,
    const char *instrument_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || instrument_id == NULL || instrument_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, instrument_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    choose_instrument(workspace, &workspace->markets[index]);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace select order operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_select_order(
    UmiTradingWorkspace *workspace,
    const char *client_order_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || client_order_id == NULL ||
        client_order_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (order_index(workspace, client_order_id) == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    copy_text(workspace->selected_order_id,
              sizeof(workspace->selected_order_id), client_order_id);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace set environment operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_environment(
    UmiTradingWorkspace *workspace,
    UmiTradingEnvironment environment)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || !valid_environment(environment))
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->environment = environment;
    workspace->draft_order.environment = environment;
    /* Apply this branch only when its contract condition is satisfied. */
    if (environment != UMI_TRADING_LIVE) workspace->live_armed = 0;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace set health operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_set_health(
    UmiTradingWorkspace *workspace,
    int market_data_ready,
    int broker_ready,
    int risk_ready)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workspace->market_data_ready = market_data_ready != 0;
    workspace->broker_ready = broker_ready != 0;
    workspace->risk_ready = risk_ready != 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace set live armed operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_live_armed(
    UmiTradingWorkspace *workspace,
    int armed)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (armed && (workspace->environment != UMI_TRADING_LIVE ||
                  !workspace->broker_ready || !workspace->risk_ready))
        return UMI_STATUS_INVALID_STATE;
    workspace->live_armed = armed != 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace set draft side operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_draft_side(
    UmiTradingWorkspace *workspace,
    UmiSide side)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || (side != UMI_SIDE_BUY && side != UMI_SIDE_SELL))
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->draft_order.side = side;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace set draft type operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_draft_type(
    UmiTradingWorkspace *workspace,
    UmiOrderType type,
    UmiTimeInForce time_in_force)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || type < UMI_ORDER_MARKET ||
        type > UMI_ORDER_STOP_LIMIT ||
        !umi_time_in_force_valid(time_in_force))
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->draft_order.type = type;
    workspace->draft_order.tif = time_in_force;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_order_type_requires_stop(type))
        workspace->draft_order.stop_price = 0.0;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace set draft quantity operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_draft_quantity(
    UmiTradingWorkspace *workspace,
    double quantity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || quantity <= 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->draft_order.quantity = quantity;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace set draft prices operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_set_draft_prices(
    UmiTradingWorkspace *workspace,
    double limit_price,
    double stop_price)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || limit_price < 0.0 || stop_price < 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->draft_order.limit_price = limit_price;
    workspace->draft_order.stop_price = stop_price;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace preview order operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_preview_order(
    UmiTradingWorkspace *workspace,
    UmiRiskDecision *out_decision)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_decision == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_order_request_validate(&workspace->draft_order);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_risk_decision_deny(&workspace->draft_risk,
                               "invalid order request");
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        workspace->draft_risk = umi_pretrade_risk_evaluate(
            &workspace->draft_order, &workspace->oms.risk_limit,
            current_position_quantity(workspace), realised_pnl(workspace));
        status = workspace->draft_risk.allowed
            ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
    }
    workspace->has_draft_risk = 1;
    workspace->revision += 1U;
    *out_decision = workspace->draft_risk;
    return status;
}

/*
 * Provide the trading workspace submit order operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_submit_order(
    UmiTradingWorkspace *workspace,
    int64_t now_ms,
    UmiRiskDecision *out_decision)
{
    UmiStatus status;
    int ready;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_decision == NULL || now_ms < 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    ready = umi_trading_health_ready(
        workspace->market_data_ready, workspace->broker_ready,
        workspace->risk_ready, workspace->environment);
    /* Apply this operation only while the related capability or state is available. */
    if (!ready) {
        umi_risk_decision_deny(out_decision,
                               "trading services are not ready");
        workspace->draft_risk = *out_decision;
        workspace->has_draft_risk = 1;
        return UMI_STATUS_UNAVAILABLE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_trading_environment_allows_live_execution(
            workspace->environment, workspace->live_armed)) {
        umi_risk_decision_deny(out_decision,
                               "live execution is not explicitly armed");
        workspace->draft_risk = *out_decision;
        workspace->has_draft_risk = 1;
        return UMI_STATUS_PERMISSION_DENIED;
    }
    (void)snprintf(workspace->draft_order.client_order_id.value,
                   sizeof(workspace->draft_order.client_order_id.value),
                   "umi-order-%llu",
                   (unsigned long long)workspace->next_order_sequence++);
    workspace->draft_order.environment = workspace->environment;
    status = umi_oms_submit(&workspace->oms, &workspace->draft_order,
                            current_position_quantity(workspace),
                            realised_pnl(workspace), now_ms, out_decision);
    workspace->draft_risk = *out_decision;
    workspace->has_draft_risk = 1;
    workspace->revision += 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        copy_text(workspace->selected_order_id,
                  sizeof(workspace->selected_order_id),
                  workspace->draft_order.client_order_id.value);
    }
    return status;
}

/*
 * Provide the trading workspace cancel selected order operation used by this module and
 * its client applications.
 */
UmiStatus umi_trading_workspace_cancel_selected_order(
    UmiTradingWorkspace *workspace)
{
    size_t index;
    UmiOrder *order;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = order_index(workspace, workspace->selected_order_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    order = &workspace->oms.orders.orders[index];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_order_transition_allowed(order->status, UMI_ORDER_CANCELLED))
        return UMI_STATUS_INVALID_STATE;
    order->status = UMI_ORDER_CANCELLED;
    order->version += 1U;
    workspace->revision += 1U;
    reconcile_selections(workspace);
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace record execution operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_record_execution(
    UmiTradingWorkspace *workspace,
    const UmiExecutionReport *report)
{
    size_t index;
    UmiOrder *order;
    UmiPosition *position = NULL;
    double remaining;
    double old_filled;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || !umi_execution_report_valid(report))
        return UMI_STATUS_INVALID_ARGUMENT;
    index = order_index(workspace, report->client_order_id.value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    order = &workspace->oms.orders.orders[index];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (order->status != UMI_ORDER_ACCEPTED &&
        order->status != UMI_ORDER_PARTIALLY_FILLED)
        return UMI_STATUS_INVALID_STATE;
    remaining = order->request.quantity - order->filled_quantity;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (report->fill_quantity > remaining) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_position_book_get(&workspace->positions,
                                   &order->request.instrument, 1, &position);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_execution_store_add(&workspace->executions, report);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_position_apply_fill(position, order->request.side,
                                     report->fill_quantity,
                                     report->fill_price);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    old_filled = order->filled_quantity;
    order->filled_quantity += report->fill_quantity;
    order->average_fill_price = order->filled_quantity > 0.0
        ? ((order->average_fill_price * old_filled) +
           (report->fill_price * report->fill_quantity)) /
              order->filled_quantity
        : 0.0;
    order->status = order->filled_quantity >= order->request.quantity
        ? UMI_ORDER_FILLED : UMI_ORDER_PARTIALLY_FILLED;
    order->version += 1U;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace engage kill switch operation used by this module and its
 * client applications.
 */
void umi_trading_workspace_engage_kill_switch(
    UmiTradingWorkspace *workspace,
    const char *reason)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return;
    umi_kill_switch_engage(&workspace->oms.kill_switch,
                           reason != NULL ? reason : "operator request");
    workspace->live_armed = 0;
    workspace->revision += 1U;
}

/*
 * Provide the trading workspace reset kill switch operation used by this module and its
 * client applications.
 */
void umi_trading_workspace_reset_kill_switch(UmiTradingWorkspace *workspace)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return;
    umi_kill_switch_reset(&workspace->oms.kill_switch);
    workspace->revision += 1U;
}

/*
 * Provide the trading workspace refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_refresh(UmiTradingWorkspace *workspace)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    reconcile_selections(workspace);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/* Add a price alert for an instrument already known to the workspace. */
UmiStatus umi_trading_workspace_add_price_alert(
    UmiTradingWorkspace *workspace,
    const char *alert_id,
    const char *instrument_id,
    UmiTradingPriceAlertDirection direction,
    double threshold,
    int64_t created_at_ms)
{
    UmiTradingPriceAlert alert;
    size_t market_position;
    UmiStatus status;

    if (workspace == NULL || instrument_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    market_position = market_index(workspace, instrument_id);
    if (market_position == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    status = umi_trading_price_alert_init(&alert,
                                          alert_id,
                                          instrument_id,
                                          direction,
                                          threshold,
                                          created_at_ms);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Seed from known market data so simply creating a rule cannot trigger it. */
    if (workspace->markets[market_position].has_quote) {
        status = umi_trading_price_alert_seed(
            &alert,
            umi_quote_mid(&workspace->markets[market_position].quote));
        if (status != UMI_STATUS_OK) {
            return status;
        }
        alert.last_observed_at_ms =
            workspace->markets[market_position].quote.event_time_ms >= 0
                ? workspace->markets[market_position].quote.event_time_ms
                : 0;
    }
    status = umi_trading_alert_book_add(&workspace->alerts, &alert);
    if (status == UMI_STATUS_OK) {
        workspace->revision += 1U;
    }
    return status;
}

/* Remove a price alert by stable identifier. */
UmiStatus umi_trading_workspace_remove_price_alert(
    UmiTradingWorkspace *workspace,
    const char *alert_id)
{
    UmiStatus status;

    if (workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_trading_alert_book_remove(&workspace->alerts, alert_id);
    if (status == UMI_STATUS_OK) {
        workspace->revision += 1U;
    }
    return status;
}

/* Enable or pause an existing price alert. */
UmiStatus umi_trading_workspace_set_price_alert_enabled(
    UmiTradingWorkspace *workspace,
    const char *alert_id,
    int enabled)
{
    UmiStatus status;

    if (workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_trading_alert_book_set_enabled(
        &workspace->alerts, alert_id, enabled);
    if (status == UMI_STATUS_OK) {
        workspace->revision += 1U;
    }
    return status;
}

/* Acknowledge an active price alert without deleting its rule. */
UmiStatus umi_trading_workspace_acknowledge_price_alert(
    UmiTradingWorkspace *workspace,
    const char *alert_id)
{
    UmiStatus status;

    if (workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_trading_alert_book_acknowledge(
        &workspace->alerts, alert_id);
    if (status == UMI_STATUS_OK) {
        workspace->revision += 1U;
    }
    return status;
}

/*
 * Provide the trading workspace snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_trading_workspace_snapshot(
    UmiTradingWorkspace *workspace,
    UmiTradingWorkspaceSnapshot *out_snapshot)
{
    UmiTradingMarketSnapshot market;
    size_t selected_order;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    reconcile_selections(workspace);
    memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_TRADING_WORKSPACE_API_VERSION;
    out_snapshot->account_id = workspace->account_id;
    out_snapshot->environment = workspace->environment;
    out_snapshot->risk_limit = workspace->oms.risk_limit;
    out_snapshot->draft_order = workspace->draft_order;
    out_snapshot->draft_risk = workspace->draft_risk;
    (void)umi_chart_workspace_snapshot(workspace->charts,
                                       &out_snapshot->charts);
    copy_text(out_snapshot->instrument_filter,
              sizeof(out_snapshot->instrument_filter),
              workspace->instrument_filter);
    out_snapshot->order_filter = workspace->order_filter;
    out_snapshot->chart_study = workspace->chart_study;
    out_snapshot->chart_study_period = workspace->chart_study_period;
    copy_text(out_snapshot->selected_instrument_id,
              sizeof(out_snapshot->selected_instrument_id),
              workspace->selected_instrument_id);
    copy_text(out_snapshot->selected_order_id,
              sizeof(out_snapshot->selected_order_id),
              workspace->selected_order_id);
    copy_text(out_snapshot->kill_switch_reason,
              sizeof(out_snapshot->kill_switch_reason),
              workspace->oms.kill_switch.reason);
    out_snapshot->watchlist_count = workspace->watchlist.count;
    out_snapshot->visible_instrument_count = visible_market_count(workspace);
    out_snapshot->market_count = workspace->market_count;
    out_snapshot->order_count = workspace->oms.orders.count;
    out_snapshot->visible_order_count = visible_order_count(workspace);
    out_snapshot->execution_count = workspace->executions.count;
    out_snapshot->position_count = workspace->positions.count;
    out_snapshot->alert_count =
        umi_trading_alert_book_count(&workspace->alerts);
    out_snapshot->active_alert_count =
        umi_trading_alert_book_active_count(&workspace->alerts);
    out_snapshot->unacknowledged_alert_count =
        umi_trading_alert_book_unacknowledged_count(&workspace->alerts);
    out_snapshot->selected_bar_count =
        umi_trading_workspace_selected_bar_count(workspace);
    out_snapshot->gross_position_quantity =
        umi_portfolio_gross_quantity(&workspace->positions);
    out_snapshot->realised_pnl = realised_pnl(workspace);
    out_snapshot->market_data_ready = workspace->market_data_ready;
    out_snapshot->broker_ready = workspace->broker_ready;
    out_snapshot->risk_ready = workspace->risk_ready;
    out_snapshot->health_ready = umi_trading_health_ready(
        workspace->market_data_ready, workspace->broker_ready,
        workspace->risk_ready, workspace->environment);
    out_snapshot->live_armed = workspace->live_armed;
    out_snapshot->kill_switch_engaged = workspace->oms.kill_switch.engaged;
    out_snapshot->has_draft_risk = workspace->has_draft_risk;
    out_snapshot->revision = workspace->revision +
        out_snapshot->charts.revision;

    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_trading_workspace_selected_market(workspace, &market) ==
        UMI_STATUS_OK) {
        out_snapshot->has_selected_instrument = 1;
        out_snapshot->has_quote = market.has_quote;
        out_snapshot->has_bar = market.has_bar;
        out_snapshot->has_depth = market.has_depth;
        /* Apply this branch only when its contract condition is satisfied. */
        if (market.has_quote) {
            out_snapshot->selected_bid = market.quote.bid;
            out_snapshot->selected_ask = market.quote.ask;
            out_snapshot->selected_mid = umi_quote_mid(&market.quote);
            out_snapshot->selected_spread = umi_quote_spread(&market.quote);
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (market.has_bar && market.previous_close > 0.0) {
            out_snapshot->selected_change =
                market.bar.close - market.previous_close;
            out_snapshot->selected_change_percent =
                out_snapshot->selected_change / market.previous_close * 100.0;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (market.has_depth) {
            out_snapshot->selected_depth_imbalance =
                umi_order_book_imbalance(&market.depth);
            out_snapshot->selected_top_liquidity =
                umi_order_book_top_liquidity(&market.depth);
        }
    }
    selected_order = order_index(workspace, workspace->selected_order_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (selected_order != SIZE_MAX) {
        UmiOrder *order = &workspace->oms.orders.orders[selected_order];
        out_snapshot->has_selected_order = 1;
        out_snapshot->can_cancel_order =
            umi_order_transition_allowed(order->status, UMI_ORDER_CANCELLED);
    }
    out_snapshot->can_preview_order =
        out_snapshot->has_selected_instrument && workspace->risk_ready;
    out_snapshot->can_submit_order = out_snapshot->can_preview_order &&
        out_snapshot->health_ready && !out_snapshot->kill_switch_engaged &&
        umi_trading_environment_allows_live_execution(
            workspace->environment, workspace->live_armed);
    out_snapshot->can_reset_kill_switch =
        out_snapshot->kill_switch_engaged;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workspace->positions.count; ++index)
        out_snapshot->revision +=
            (uint64_t)(workspace->positions.positions[index].quantity != 0.0);
    return UMI_STATUS_OK;
}

/*
 * Find trading workspace visible instrument while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_trading_workspace_visible_instrument_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiTradingMarketSnapshot *out_market)
{
    size_t source_index;
    size_t visible_index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_market == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (source_index = 0U; source_index < workspace->market_count;
         ++source_index) {
        /* Apply this operation only while the related capability or state is available. */
        if (!market_visible(workspace, &workspace->markets[source_index]))
            continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (visible_index == index) {
            *out_market = workspace->markets[source_index];
            return UMI_STATUS_OK;
        }
        visible_index += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the trading workspace selected market operation used by this module and its
 * client applications.
 */
UmiStatus umi_trading_workspace_selected_market(
    UmiTradingWorkspace *workspace,
    UmiTradingMarketSnapshot *out_market)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_market == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, workspace->selected_instrument_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_market = workspace->markets[index];
    return UMI_STATUS_OK;
}

/* Return the selected series length without exposing its mutable storage. */
size_t umi_trading_workspace_selected_bar_count(
    const UmiTradingWorkspace *workspace)
{
    size_t index;

    if (workspace == NULL) return 0U;
    index = market_index(
        workspace,
        workspace->selected_instrument_id);
    return index != SIZE_MAX ? workspace->bar_histories[index].count : 0U;
}

/* Copy one selected candle in oldest-to-newest order for deterministic plots,
 * exports, studies, and automation clients. */
UmiStatus umi_trading_workspace_selected_bar_at(
    const UmiTradingWorkspace *workspace,
    size_t index,
    UmiBar *out_bar)
{
    size_t market_position;

    if (workspace == NULL || out_bar == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    market_position = market_index(
        workspace,
        workspace->selected_instrument_id);
    if (market_position == SIZE_MAX ||
        index >= workspace->bar_histories[market_position].count) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_bar = workspace->bar_histories[market_position].bars[index];
    return UMI_STATUS_OK;
}

/* Copy one price alert by position for presentation or persistence. */
UmiStatus umi_trading_workspace_price_alert_at(
    const UmiTradingWorkspace *workspace,
    size_t index,
    UmiTradingPriceAlert *out_alert)
{
    if (workspace == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_trading_alert_book_at(&workspace->alerts, index, out_alert);
}

/*
 * Find trading workspace visible order while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_trading_workspace_visible_order_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiOrder *out_order)
{
    size_t source_index;
    size_t visible_index = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_order == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (source_index = workspace->oms.orders.count; source_index > 0U;
         --source_index) {
        UmiOrder *order = &workspace->oms.orders.orders[source_index - 1U];
        /* Apply this operation only while the related capability or state is available. */
        if (!order_visible(workspace, order)) continue;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (visible_index == index) {
            *out_order = *order;
            return UMI_STATUS_OK;
        }
        visible_index += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find trading workspace position while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_trading_workspace_position_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiPosition *out_position)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_position == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= workspace->positions.count) return UMI_STATUS_NOT_FOUND;
    *out_position = workspace->positions.positions[index];
    return UMI_STATUS_OK;
}

/*
 * Find trading workspace execution while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_trading_workspace_execution_at(
    UmiTradingWorkspace *workspace,
    size_t newest_first_index,
    UmiExecutionReport *out_report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (newest_first_index >= workspace->executions.count)
        return UMI_STATUS_NOT_FOUND;
    *out_report = workspace->executions.reports[
        workspace->executions.count - newest_first_index - 1U];
    return UMI_STATUS_OK;
}

/*
 * Provide the trading workspace charts operation used by this module and its client
 * applications.
 */
UmiChartWorkspace *umi_trading_workspace_charts(
    UmiTradingWorkspace *workspace)
{
    return workspace != NULL ? workspace->charts : NULL;
}

/*
 * Provide the trading environment text operation used by this module and its client
 * applications.
 */
const char *umi_trading_environment_text(UmiTradingEnvironment environment)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (environment) {
        case UMI_TRADING_PAPER: return "paper";
        case UMI_TRADING_LIVE: return "live";
        case UMI_TRADING_SIMULATION:
        default: return "simulation";
    }
}

/* Provide the trading side text operation used by this module and its client applications. */
const char *umi_trading_side_text(UmiSide side)
{
    return side == UMI_SIDE_SELL ? "sell" : "buy";
}

/*
 * Provide the trading order type text operation used by this module and its client
 * applications.
 */
const char *umi_trading_order_type_text(UmiOrderType type)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (type) {
        case UMI_ORDER_LIMIT: return "limit";
        case UMI_ORDER_STOP: return "stop";
        case UMI_ORDER_STOP_LIMIT: return "stop-limit";
        case UMI_ORDER_MARKET:
        default: return "market";
    }
}

/*
 * Provide the trading time in force text operation used by this module and its client
 * applications.
 */
const char *umi_trading_time_in_force_text(UmiTimeInForce time_in_force)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (time_in_force) {
        case UMI_TIF_GTC: return "GTC";
        case UMI_TIF_IOC: return "IOC";
        case UMI_TIF_FOK: return "FOK";
        case UMI_TIF_DAY:
        default: return "DAY";
    }
}

/*
 * Provide the trading order status text operation used by this module and its client
 * applications.
 */
const char *umi_trading_order_status_text(UmiOrderStatus status)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (status) {
        case UMI_ORDER_VALIDATED: return "validated";
        case UMI_ORDER_ACCEPTED: return "accepted";
        case UMI_ORDER_PARTIALLY_FILLED: return "partially-filled";
        case UMI_ORDER_FILLED: return "filled";
        case UMI_ORDER_CANCELLED: return "cancelled";
        case UMI_ORDER_REJECTED: return "rejected";
        case UMI_ORDER_NEW:
        default: return "new";
    }
}

/*
 * Provide the trading market state text operation used by this module and its client
 * applications.
 */
const char *umi_trading_market_state_text(UmiMarketState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_MARKET_PREOPEN: return "pre-open";
        case UMI_MARKET_OPEN: return "open";
        case UMI_MARKET_HALTED: return "halted";
        case UMI_MARKET_CLOSED:
        default: return "closed";
    }
}

/*
 * Provide the trading workspace order filter text operation used by this module and its
 * client applications.
 */
const char *umi_trading_workspace_order_filter_text(
    UmiTradingWorkspaceOrderFilter order_filter)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (order_filter) {
        case UMI_TRADING_WORKSPACE_ORDERS_OPEN: return "open";
        case UMI_TRADING_WORKSPACE_ORDERS_FILLED: return "filled";
        case UMI_TRADING_WORKSPACE_ORDERS_CANCELLED: return "cancelled";
        case UMI_TRADING_WORKSPACE_ORDERS_REJECTED: return "rejected";
        case UMI_TRADING_WORKSPACE_ORDERS_ALL:
        default: return "all";
    }
}
