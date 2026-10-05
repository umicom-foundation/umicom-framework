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
#include "umicom/trading/chart_history.h"
#include "umicom/chart/drawing_visibility.h"

#include <ctype.h>
#include <stdatomic.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/finance/identifier.h"
#include "umicom/trading/bar.h"
#include "umicom/trading/depth.h"
#include "umicom/trading/environment.h"
#include "umicom/trading/execution_report.h"
#include "umicom/trading/fill.h"
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
    UmiChartNavigation chart_navigation[UMI_TRADING_MAX_WATCHLIST];
    size_t market_count;
    UmiOms oms;
    UmiExecutionStore executions;
    UmiPositionBook positions;
    UmiTradingAlertBook alerts;
    UmiTradingTradeTape *trade_tape;
    UmiChartWorkspace *charts;
    UmiChartDrawingHistory *drawing_history;
    UmiOrderRequest draft_order;
    UmiRiskDecision draft_risk;
    UmiRiskPricePolicy pricePolicy;
    UmiPretradeRiskEvidence riskEvidence;
    char instrument_filter[UMI_TRADING_WORKSPACE_FILTER_CAPACITY];
    char order_search[UMI_TRADING_WORKSPACE_FILTER_CAPACITY];
    UmiTradingWorkspaceOrderFilter order_filter;
    UmiTradingChartStudy chart_study;
    size_t chart_study_period;
    char selected_instrument_id[UMI_FINANCE_ID_CAPACITY];
    char selected_order_id[UMI_FINANCE_ID_CAPACITY];
    uint64_t next_order_sequence;
    uint64_t revision;
    uint64_t session_report_owner_id;
    int market_data_ready;
    int broker_ready;
    int risk_ready;
    int live_armed;
    int has_draft_risk;
};

/* Reports may outlive a workspace allocation. A process-local identity avoids
 * mistaking a new owner at the same address/revision for the captured book.
 * This is lifecycle identity, not authentication or persistent provenance. */
static atomic_uint_fast64_t next_session_report_owner = 1;
static UmiStatus AllocateSessionReportOwner(uint64_t *out)
{
    uint_fast64_t next = atomic_load_explicit(&next_session_report_owner, memory_order_relaxed);
    for (;;) {
        if (next >= UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
        if (atomic_compare_exchange_weak_explicit(&next_session_report_owner, &next, next + 1U,
            memory_order_relaxed, memory_order_relaxed)) { *out = (uint64_t)next; return UMI_STATUS_OK; }
    }
}

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
    /* Identity and status belong to one canonical projection. Filtering never
     * modifies executions, positions, the ticket or linked market context. */
    if (!contains_case_insensitive(order->request.client_order_id.value, workspace->order_search) &&
        !contains_case_insensitive(order->request.instrument.instrument_id.value, workspace->order_search) &&
        !contains_case_insensitive(order->request.instrument.symbol, workspace->order_search) &&
        !contains_case_insensitive(order->request.instrument.venue, workspace->order_search)) {
        return 0;
    }

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
    status = AllocateSessionReportOwner(&workspace->session_report_owner_id);
    if (status != UMI_STATUS_OK) { free(workspace); return status; }
    workspace->account_id = effective.account_id;
    workspace->environment = effective.environment;
    workspace->order_filter = UMI_TRADING_WORKSPACE_ORDERS_ALL;
    workspace->next_order_sequence = 1U;
    workspace->revision = 1U;
    workspace->risk_ready = 1;
    workspace->pricePolicy = UmiRiskPricePolicyDefault();
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
    /* Allocate the public trade tape separately because its fixed history is
     * deliberately much larger than the frequently copied workspace state. */
    status = umi_trading_trade_tape_create(&workspace->trade_tape);
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
    umi_trading_trade_tape_destroy(workspace->trade_tape);
    workspace->trade_tape = NULL;
    UmiChartDrawingHistoryDestroy(workspace->drawing_history);
    umi_chart_workspace_destroy(workspace->charts);
    workspace->charts = NULL;
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
    /* Purge retained public prints before removing the market identity so a
     * later instrument cannot inherit the removed symbol's Time and Sales. */
    if (umi_trading_trade_tape_remove_instrument(
            workspace->trade_tape, instrument_id) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
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
    if (workspace == NULL || quote == NULL || !umi_quote_valid(quote) ||
        !umi_instrument_valid(&quote->instrument))
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, quote->instrument.instrument_id.value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (!UmiRiskInstrumentMatches(&workspace->markets[index].instrument, &quote->instrument))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Display-only legacy quotes may omit time. Once timestamped evidence is
     * present, an older event must not roll it back or fire alerts again. */
    if (workspace->markets[index].has_quote &&
        quote->event_time_ms < workspace->markets[index].quote.event_time_ms)
        return UMI_STATUS_INVALID_STATE;

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
        !umi_financial_id_valid(&bar->instrument.instrument_id) ||
        !isfinite(previous_close) || previous_close < 0.0)
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

/* Accept one public market trade through the sequence-checked shared tape and
 * mirror only its latest value into the matching lightweight market row. */
UmiStatus umi_trading_workspace_update_trade(
    UmiTradingWorkspace *workspace,
    const UmiTradingTradeTapeRecord *record)
{
    size_t index;
    UmiStatus status;

    if (workspace == NULL || record == NULL ||
        !umi_trade_tick_valid(&record->trade) ||
        !umi_instrument_valid(&record->trade.instrument)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    index = market_index(
        workspace, record->trade.instrument.instrument_id.value);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    status = umi_trading_trade_tape_append(workspace->trade_tape, record);
    if (status != UMI_STATUS_OK) return status;
    workspace->markets[index].trade = record->trade;
    workspace->markets[index].trade_sequence = record->sequence;
    workspace->markets[index].trade_direction = record->direction;
    workspace->markets[index].has_trade = 1;
    workspace->markets[index].revision += 1U;
    workspace->market_data_ready = 1;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

/* Keep feed availability separate from general quote health so an application
 * can honestly show a missing trade provider while charts still receive bars. */
UmiStatus umi_trading_workspace_set_trade_tape_provider_ready(
    UmiTradingWorkspace *workspace,
    int ready)
{
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_trade_tape_set_provider_ready(
        workspace->trade_tape, ready);
    if (status == UMI_STATUS_OK) workspace->revision += 1U;
    return status;
}

/* Store Time and Sales filters in Framework state so all open panels and
 * frontend technologies present exactly the same selected rows. */
UmiStatus umi_trading_workspace_set_trade_tape_filter(
    UmiTradingWorkspace *workspace,
    UmiTradingTradeTapeFilter filter,
    double minimum_size)
{
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_trade_tape_set_filter(
        workspace->trade_tape, filter, minimum_size);
    if (status == UMI_STATUS_OK) workspace->revision += 1U;
    return status;
}

/* Pause only the visible trade sequence; provider ingestion remains active so
 * a user can inspect a stable row and later resume without a data gap. */
UmiStatus umi_trading_workspace_set_trade_tape_paused(
    UmiTradingWorkspace *workspace,
    int paused)
{
    UmiStatus status;

    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_trading_trade_tape_set_paused(
        workspace->trade_tape, paused);
    if (status == UMI_STATUS_OK) workspace->revision += 1U;
    return status;
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
/* The status-only assignment is superseded by the shared atomic order query below, preserving the current text filter. The superseded implementation is retained for engineering review. */
#if 0
    workspace->order_filter = order_filter;
    workspace->revision += 1U;
    reconcile_selections(workspace);
    return UMI_STATUS_OK;
#endif
    /* Reuse atomic query validation so old status-only callers retain the
     * current search text and gain the same no-op/overflow handling. */
    UmiTradingOrderQuery query;
    query.status = order_filter;
    memcpy(query.text, workspace->order_search, sizeof(query.text));
    return UmiTradingWorkspaceSetOrderQuery(workspace, &query);
}

/* Persist a bounded study choice in toolkit-neutral workspace state. */
UmiStatus umi_trading_workspace_set_chart_study(
    UmiTradingWorkspace *workspace,
    UmiTradingChartStudy study,
    size_t period)
{
/* The workspace now accepts Framework candle studies alongside the existing averages.
 * The previous implementation is retained for engineering review. */
#if 0
    if (workspace == NULL ||
        (study != UMI_TRADING_CHART_STUDY_NONE &&
         study != UMI_TRADING_CHART_STUDY_SIMPLE_AVERAGE &&
         study != UMI_TRADING_CHART_STUDY_EXPONENTIAL_AVERAGE) ||
        period < 2U || period > 200U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#endif
    if (workspace == NULL ||
        (study != UMI_TRADING_CHART_STUDY_NONE &&
         study != UMI_TRADING_CHART_STUDY_SIMPLE_AVERAGE &&
         study != UMI_TRADING_CHART_STUDY_EXPONENTIAL_AVERAGE &&
         study != UMI_TRADING_CHART_STUDY_VOLUME_WEIGHTED &&
         study != UMI_TRADING_CHART_STUDY_BOLLINGER &&
         study != UMI_TRADING_CHART_STUDY_DONCHIAN &&
         study != UMI_TRADING_CHART_STUDY_VOLUME_PROFILE) ||
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
    if (workspace == NULL || !isfinite(quantity) || quantity <= 0.0)
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
    if (workspace == NULL || !isfinite(limit_price) || !isfinite(stop_price) ||
        limit_price < 0.0 || stop_price < 0.0)
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
        memset(&workspace->riskEvidence, 0, sizeof workspace->riskEvidence);
        umi_risk_decision_deny(&workspace->draft_risk,
                               "invalid order request");
        workspace->riskEvidence.decision = workspace->draft_risk;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        workspace->draft_risk = UmiPretradeRiskEvaluateQuoted(
            &workspace->draft_order, &workspace->oms.risk_limit,
            current_position_quantity(workspace), realised_pnl(workspace),
            NULL, 0, &workspace->pricePolicy, &workspace->riskEvidence);
        status = workspace->draft_risk.allowed
            ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
    }
    workspace->has_draft_risk = 1;
    workspace->revision += 1U;
    *out_decision = workspace->draft_risk;
    return status;
}

/* Price/time policy is copied into the workspace; it does not grant trust,
 * broker readiness, live arming or any additional permission. */
UmiStatus UmiTradingWorkspaceSetRiskPricePolicy(UmiTradingWorkspace *workspace,
    const UmiRiskPricePolicy *policy)
{
    if (workspace == NULL || !UmiRiskPricePolicyValid(policy))
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->pricePolicy = *policy;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus UmiTradingWorkspaceRiskEvidence(const UmiTradingWorkspace *workspace,
    UmiPretradeRiskEvidence *outEvidence)
{
    if (workspace == NULL || outEvidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!workspace->has_draft_risk) return UMI_STATUS_NOT_FOUND;
    *outEvidence = workspace->riskEvidence;
    outEvidence->decision = workspace->draft_risk;
    return UMI_STATUS_OK;
}

/* The same timestamp used by a live provider or replay must be supplied by
 * the caller. A preview is retained evidence, never a cached submit approval. */
UmiStatus UmiTradingWorkspacePreviewOrderAt(UmiTradingWorkspace *workspace,
    int64_t nowMs, UmiRiskDecision *outDecision)
{
    if (workspace == NULL || outDecision == NULL || nowMs < 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    const size_t index = market_index(workspace, workspace->selected_instrument_id);
    const UmiQuote *quote = index != SIZE_MAX && workspace->markets[index].has_quote
        ? &workspace->markets[index].quote : NULL;
    workspace->draft_risk = UmiPretradeRiskEvaluateQuoted(&workspace->draft_order,
        &workspace->oms.risk_limit, current_position_quantity(workspace),
        realised_pnl(workspace), quote, nowMs, &workspace->pricePolicy,
        &workspace->riskEvidence);
    workspace->has_draft_risk = 1;
    workspace->revision += 1U;
    *outDecision = workspace->draft_risk;
    return outDecision->allowed ? UMI_STATUS_OK : UMI_STATUS_PERMISSION_DENIED;
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
    memset(&workspace->riskEvidence, 0, sizeof workspace->riskEvidence);
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
    const size_t marketPosition = market_index(workspace, workspace->selected_instrument_id);
    const UmiQuote *quote = marketPosition != SIZE_MAX && workspace->markets[marketPosition].has_quote
        ? &workspace->markets[marketPosition].quote : NULL;
    status = UmiOmsSubmitQuoted(&workspace->oms, &workspace->draft_order,
        current_position_quantity(workspace), realised_pnl(workspace), now_ms,
        quote, &workspace->pricePolicy, out_decision, &workspace->riskEvidence);
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
    const UmiExecutionReport *retained = NULL;
    UmiOrder candidateOrder;
    UmiPosition candidatePosition;
    int createPosition = 0;
    UmiStatus status;

    /* Validate before comparing IDs or inspecting any mutable order state. */
    if (workspace == NULL || !umi_execution_report_valid(report))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiExecutionStoreFind(&workspace->executions, &report->execution_id, &retained);
    if (status == UMI_STATUS_OK) {
        /* An exact replay remains harmless even after the order is filled or
         * cancelled. Reuse of an ID for different economics is a conflict. */
        return UmiExecutionReportEqual(retained, report)
            ? UMI_STATUS_OK : UMI_STATUS_ALREADY_EXISTS;
    }
    if (status != UMI_STATUS_NOT_FOUND) return status;
    if (workspace->executions.count >= UMI_TRADING_MAX_ORDERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    index = order_index(workspace, report->client_order_id.value);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    order = &workspace->oms.orders.orders[index];
    if (report->fill_quantity > order->request.quantity - order->filled_quantity)
        return UMI_STATUS_INVALID_ARGUMENT;
    candidateOrder = *order;
    status = umi_order_apply_execution(&candidateOrder, report);
    if (status != UMI_STATUS_OK) return status;
    status = umi_position_book_get(&workspace->positions,
        &order->request.instrument, 0, &position);
    if (status == UMI_STATUS_NOT_FOUND) {
        if (workspace->positions.count >= UMI_TRADING_MAX_POSITIONS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        candidatePosition = (UmiPosition){0};
        candidatePosition.instrument = order->request.instrument;
        createPosition = 1;
    } else if (status == UMI_STATUS_OK) {
        if (!UmiRiskInstrumentMatches(&position->instrument, &order->request.instrument))
            return UMI_STATUS_INVALID_STATE;
        candidatePosition = *position;
    } else {
        return status;
    }
    status = umi_position_apply_fill(&candidatePosition, order->request.side,
        report->fill_quantity, report->fill_price);
    if (status != UMI_STATUS_OK) return status;
    status = umi_execution_store_add(&workspace->executions, report);
    if (status != UMI_STATUS_OK) return status;
    /* No fallible work remains. One workspace owner commits this in-memory
     * transaction; cross-process persistence remains a Data Server concern. */
    if (createPosition) position = &workspace->positions.positions[workspace->positions.count++];
    *position = candidatePosition;
    *order = candidateOrder;
    workspace->has_draft_risk = 0;
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
    /* The tape snapshot reports whole-feed health while selected count and
     * latest trade describe the instrument followed by linked panels. */
    if (umi_trading_trade_tape_snapshot(
            workspace->trade_tape, &out_snapshot->trade_tape) !=
        UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_STATE;
    }
    out_snapshot->selected_trade_count =
        umi_trading_workspace_selected_trade_count(workspace);
    if (out_snapshot->selected_trade_count > 0U &&
        umi_trading_workspace_selected_trade_at(
            workspace, 0U, &out_snapshot->selected_latest_trade) ==
            UMI_STATUS_OK) {
        out_snapshot->has_selected_trade = 1;
    }
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
        out_snapshot->charts.revision + out_snapshot->trade_tape.revision;

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

/* Count filtered Time and Sales rows for the selected instrument without
 * exposing the tape's internal bounded storage. */
size_t umi_trading_workspace_selected_trade_count(
    const UmiTradingWorkspace *workspace)
{
    if (workspace == NULL || workspace->selected_instrument_id[0] == '\0') {
        return 0U;
    }
    return umi_trading_trade_tape_visible_count(
        workspace->trade_tape, workspace->selected_instrument_id);
}

/* Copy one selected Time and Sales row in newest-first order so table, export,
 * automation, and accessibility clients all observe the same ordering. */
UmiStatus umi_trading_workspace_selected_trade_at(
    const UmiTradingWorkspace *workspace,
    size_t newest_first_index,
    UmiTradingTradeTapeRecord *out_record)
{
    if (workspace == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (workspace->selected_instrument_id[0] == '\0') {
        return UMI_STATUS_NOT_FOUND;
    }
    return umi_trading_trade_tape_visible_at(
        workspace->trade_tape,
        workspace->selected_instrument_id,
        newest_first_index,
        out_record);
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

/* An explicit reset affects the private ticket, never an order in the OMS. */
UmiStatus UmiTradingWorkspaceResetDraft(UmiTradingWorkspace *workspace)
{
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t selected = market_index(workspace, workspace->selected_instrument_id);
    initialise_draft(workspace);
    if (selected != SIZE_MAX) choose_instrument(workspace, &workspace->markets[selected]);
    memset(&workspace->riskEvidence, 0, sizeof workspace->riskEvidence);
    umi_risk_decision_deny(&workspace->draft_risk,
        "Ticket reset. Preview risk before submitting.");
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}


/* Keep matching, selection reconciliation and version checks in the domain
 * owner; products and native adapters never maintain a second order book. */
UmiStatus UmiTradingWorkspaceSetOrderQuery(UmiTradingWorkspace *workspace,
    const UmiTradingOrderQuery *query)
{
    if (workspace == NULL || query == NULL || !valid_order_filter(query->status) ||
        memchr(query->text, '\0', sizeof(query->text)) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->order_filter == query->status &&
        strcmp(workspace->order_search, query->text) == 0) return UMI_STATUS_OK;
    if (workspace->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    workspace->order_filter = query->status;
    copy_text(workspace->order_search, sizeof(workspace->order_search), query->text);
    ++workspace->revision;
    reconcile_selections(workspace);
    return UMI_STATUS_OK;
}

UmiStatus UmiTradingWorkspaceGetOrderQuery(const UmiTradingWorkspace *workspace,
    UmiTradingOrderQuery *out_query)
{
    if (workspace == NULL || out_query == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiTradingOrderQuery value = {0};
    value.status = workspace->order_filter;
    memcpy(value.text, workspace->order_search, sizeof(value.text));
    *out_query = value;
    return UMI_STATUS_OK;
}

static int OrderReviewIdValid(const char *id)
{
    if (id == NULL) return 0;
    for (size_t i = 0U; i < UMI_FINANCE_ID_CAPACITY; ++i) {
        if (id[i] == '\0') return i != 0U;
    }
    return 0;
}

UmiStatus UmiTradingWorkspaceReviewOrder(const UmiTradingWorkspace *workspace,
    const char *client_order_id, UmiTradingOrderReview *out_review)
{
    if (workspace == NULL || out_review == NULL || !OrderReviewIdValid(client_order_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = order_index(workspace, client_order_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiTradingOrderReview value = {0};
    value.order = workspace->oms.orders.orders[index];
    value.workspace_revision = workspace->revision;
    value.can_cancel = order_visible(workspace, &value.order) &&
        strcmp(client_order_id, workspace->selected_order_id) == 0 &&
        umi_order_transition_allowed(value.order.status, UMI_ORDER_CANCELLED);
    for (size_t i = 0U; i < workspace->executions.count; ++i) {
        const UmiExecutionReport *fill = &workspace->executions.reports[i];
        if (strcmp(fill->client_order_id.value, client_order_id) == 0)
            value.executions[value.execution_count++] = *fill;
    }
    *out_review = value;
    return UMI_STATUS_OK;
}

UmiStatus UmiTradingWorkspaceCancelReviewedOrder(UmiTradingWorkspace *workspace,
    const char *client_order_id, uint64_t expected_order_version)
{
    if (workspace == NULL || !OrderReviewIdValid(client_order_id)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = order_index(workspace, client_order_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const UmiOrder *order = &workspace->oms.orders.orders[index];
    if (strcmp(client_order_id, workspace->selected_order_id) != 0 ||
        order->version != expected_order_version || !order_visible(workspace, order))
        return UMI_STATUS_INVALID_STATE;
    if (order->version == UINT64_MAX || workspace->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return umi_trading_workspace_cancel_selected_order(workspace);
}


/* Chart edits belong to the canonical workspace rather than a transient GTK
 * widget. Capture symbol identity before any gesture crosses a redraw. */
static UmiStatus ChartSelectedIndex(const UmiTradingWorkspace *workspace,
    const char *instrument_id, size_t *out_index)
{
    if (workspace == NULL || instrument_id == NULL || instrument_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(workspace->selected_instrument_id, instrument_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    size_t index = market_index(workspace, instrument_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_index = index;
    return UMI_STATUS_OK;
}
UmiStatus UmiTradingWorkspaceGetChartNavigation(const UmiTradingWorkspace *workspace,
    const char *instrument_id, UmiChartNavigation *out_navigation)
{
    if (out_navigation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    UmiStatus status = ChartSelectedIndex(workspace, instrument_id, &index);
    if (status == UMI_STATUS_OK) *out_navigation = workspace->chart_navigation[index];
    return status;
}
UmiStatus UmiTradingWorkspaceSetChartNavigation(UmiTradingWorkspace *workspace,
    const char *instrument_id, const UmiChartNavigation *navigation)
{
    if (navigation == NULL || navigation->visible_bars > UMI_CHART_MAX_POINTS ||
        (navigation->pinned != 0 && navigation->pinned != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    if (!UmiChartTimeframeValid(navigation->interval_ms)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    UmiStatus status = ChartSelectedIndex(workspace, instrument_id, &index);
    if (status != UMI_STATUS_OK) return status;
    UmiChartNavigation *current = &workspace->chart_navigation[index];
/* The timeframe is part of the view identity; changing it must advance the canonical workspace revision. The previous implementation remains for engineering review. */
#if 0
    if (current->visible_bars == navigation->visible_bars && current->anchor_ms == navigation->anchor_ms &&
        current->pinned == navigation->pinned) return UMI_STATUS_OK;
#endif
    if (current->visible_bars == navigation->visible_bars && current->anchor_ms == navigation->anchor_ms &&
        current->pinned == navigation->pinned && current->interval_ms == navigation->interval_ms) return UMI_STATUS_OK;
    if (workspace->revision == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
    *current = *navigation;
    workspace->revision++;
    return UMI_STATUS_OK;
}
/* Drawing construction now uses the shared tool contract for ranges, liquidity annotations and directional rays. The reusable identity search also serves guarded duplicate actions, while preserving restored identities and the existing drawing API. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiTradingWorkspaceAddChartDrawing(UmiTradingWorkspace *workspace,
    const char *instrument_id, const char *tool, UmiChartPoint first, UmiChartPoint second)
{
    if (tool == NULL || (strcmp(tool, "support") != 0 && strcmp(tool, "resistance") != 0 &&
        strcmp(tool, "trend") != 0) || first.time_ms < 0 || second.time_ms < 0 ||
        !isfinite(first.value) || !isfinite(second.value) ||
        (strcmp(tool, "trend") == 0 && first.time_ms == second.time_ms))
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    UmiStatus status = ChartSelectedIndex(workspace, instrument_id, &index);
    if (status != UMI_STATUS_OK) return status;
    (void)index;
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(workspace->charts);
    uint64_t revision = umi_chart_drawing_registry_revision(registry);
    if (workspace->revision == UINT64_MAX || revision == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingSnapshot drawing = {0}, existing;
/* Restored drawing identities may exceed the local revision counter. A bounded identity search now permits subsequent drawings without changing restored IDs. The previous implementation remains for engineering review. */
#if 0
    (void)snprintf(drawing.id, sizeof drawing.id, "trading-drawing-%llu", (unsigned long long)(revision + 1U));
    if (umi_chart_drawing_registry_find(registry, drawing.id, &existing) == UMI_STATUS_OK)
        return UMI_STATUS_ALREADY_EXISTS;
#endif
    /* Restored drawings retain their IDs. A bounded search avoids a collision
     * when a saved ID is ahead of this session's local registry revision. */
    uint64_t candidate = revision + 1U;
    for (size_t attempt = 0U; ; ++attempt) {
        (void)snprintf(drawing.id, sizeof drawing.id, "trading-drawing-%llu", (unsigned long long)candidate);
        status = umi_chart_drawing_registry_find(registry, drawing.id, &existing);
        if (status == UMI_STATUS_NOT_FOUND) break;
        if (status != UMI_STATUS_OK) return status;
        if (candidate == UINT64_MAX || attempt >= UMI_CHART_DRAWING_CAPACITY)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        ++candidate;
    }
    copy_text(drawing.pane_id, sizeof drawing.pane_id, instrument_id);
    copy_text(drawing.tool, sizeof drawing.tool, tool);
    drawing.time1 = first.time_ms;
    drawing.time2 = second.time_ms;
    drawing.value1 = first.value;
    drawing.value2 = strcmp(tool, "trend") == 0 ? second.value : first.value;
    status = umi_chart_drawing_registry_upsert(registry, &drawing);
    if (status == UMI_STATUS_OK) workspace->revision++;
    return status;
}
#endif
#include "umicom/chart/drawing_edit.h"
/* Chart identities are shared by new gestures and explicit copies. Restored
 * identities are never overwritten, even when ahead of the local revision. */
static UmiStatus NextChartDrawingId(UmiChartDrawingRegistry *registry,char out[128])
{
    uint64_t revision=umi_chart_drawing_registry_revision(registry);
    if(revision==UINT64_MAX)return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingSnapshot existing;uint64_t candidate=revision+1U;
    for(size_t attempt=0;;++attempt){
        (void)snprintf(out,128,"trading-drawing-%llu",(unsigned long long)candidate);
        UmiStatus status=umi_chart_drawing_registry_find(registry,out,&existing);
        if(status==UMI_STATUS_NOT_FOUND)return UMI_STATUS_OK;
        if(status!=UMI_STATUS_OK)return status;
        if(candidate==UINT64_MAX||attempt>=UMI_CHART_DRAWING_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;
        ++candidate;
    }
}
#include "chart_drawing_history.inc"
/* Canonical drawing edits now publish their reversible evidence with the registry, preserving atomic ownership in Framework. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiTradingWorkspaceAddChartDrawing(UmiTradingWorkspace *workspace,
    const char *instrument_id,const char *tool,UmiChartPoint first,UmiChartPoint second)
{
    UmiChartDrawingKind kind;UmiStatus status=UmiChartDrawingKindParse(tool,&kind);
    if(status!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve argument validation before workspace access. */
    UmiChartDrawingSnapshot drawing;
    status=UmiChartDrawingInitialize("candidate","candidate",kind,first,second,&drawing);
    if(status!=UMI_STATUS_OK)return status;
    size_t index;status=ChartSelectedIndex(workspace,instrument_id,&index);
    if(status!=UMI_STATUS_OK)return status;
    (void)index;
    if(workspace->revision==UINT64_MAX)return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingRegistry *registry=umi_chart_workspace_drawings(workspace->charts);char id[128];
    status=NextChartDrawingId(registry,id);
    if(status==UMI_STATUS_OK)status=UmiChartDrawingInitialize(id,instrument_id,kind,first,second,&drawing);
    if(status==UMI_STATUS_OK)status=umi_chart_drawing_registry_upsert(registry,&drawing);
    if(status==UMI_STATUS_OK)workspace->revision++;
    return status;
}
#endif
UmiStatus UmiTradingWorkspaceAddChartDrawing(UmiTradingWorkspace *workspace,
    const char *instrument_id,const char *tool,UmiChartPoint first,UmiChartPoint second)
{
    UmiChartDrawingKind kind; UmiChartDrawingSnapshot drawing;
    UmiStatus status = UmiChartDrawingKindParse(tool, &kind);
    if (status != UMI_STATUS_OK) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiChartDrawingInitialize("candidate", "candidate", kind, first, second, &drawing);
    if (status != UMI_STATUS_OK) return status;
    ChartHistoryRequest request = {.action=CH_ADD, .pane=instrument_id, .kind=kind, .first=first, .second=second};
    return ChartHistoryApply(workspace, &request, "Create drawing");
}
/* Canonical drawing edits now publish their reversible evidence with the registry, preserving atomic ownership in Framework. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiTradingWorkspaceSetChartDrawingLocked(UmiTradingWorkspace *workspace,
    const char *instrumentId,const char *drawingId,uint64_t expectedRevision,int locked)
{
    size_t index;UmiStatus status=ChartSelectedIndex(workspace,instrumentId,&index);
    if(status!=UMI_STATUS_OK)return status;
    (void)index;
    if(workspace->revision==UINT64_MAX)return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingRegistry *registry=umi_chart_workspace_drawings(workspace->charts);
    uint64_t before=umi_chart_drawing_registry_revision(registry);
    status=UmiChartDrawingSetLocked(registry,instrumentId,drawingId,expectedRevision,locked);
    if(status==UMI_STATUS_OK&&before!=umi_chart_drawing_registry_revision(registry))workspace->revision++;
    return status;
}
#endif
UmiStatus UmiTradingWorkspaceSetChartDrawingLocked(UmiTradingWorkspace *workspace,
    const char *instrumentId,const char *drawingId,uint64_t expectedRevision,int locked)
{
    ChartHistoryRequest request = {.action=CH_LOCK, .flag=locked, .pane=instrumentId, .id=drawingId, .expected=expectedRevision};
    return ChartHistoryApply(workspace, &request, "Change lock");
}
/* Canonical drawing edits now publish their reversible evidence with the registry, preserving atomic ownership in Framework. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiTradingWorkspaceMoveChartDrawing(UmiTradingWorkspace *workspace,
    const char *instrumentId,const char *drawingId,uint64_t expectedRevision,UmiChartPoint first,UmiChartPoint second)
{
    size_t index;UmiStatus status=ChartSelectedIndex(workspace,instrumentId,&index);
    if(status!=UMI_STATUS_OK)return status;
    (void)index;
    if(workspace->revision==UINT64_MAX)return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingRegistry *registry=umi_chart_workspace_drawings(workspace->charts);
    uint64_t before=umi_chart_drawing_registry_revision(registry);
    status=UmiChartDrawingSetGeometry(registry,instrumentId,drawingId,expectedRevision,first,second);
    if(status==UMI_STATUS_OK&&before!=umi_chart_drawing_registry_revision(registry))workspace->revision++;
    return status;
}
#endif
UmiStatus UmiTradingWorkspaceMoveChartDrawing(UmiTradingWorkspace *workspace,
    const char *instrumentId,const char *drawingId,uint64_t expectedRevision,UmiChartPoint first,UmiChartPoint second)
{
    ChartHistoryRequest request = {.action=CH_MOVE, .first=first, .second=second, .pane=instrumentId, .id=drawingId, .expected=expectedRevision};
    return ChartHistoryApply(workspace, &request, "Move drawing");
}
/* Canonical drawing edits now publish their reversible evidence with the registry, preserving atomic ownership in Framework. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiTradingWorkspaceDuplicateChartDrawing(UmiTradingWorkspace *workspace,
    const char *instrumentId,const char *drawingId,uint64_t expectedRevision,char *outId,size_t capacity)
{
    if(outId==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;UmiStatus status=ChartSelectedIndex(workspace,instrumentId,&index);
    if(status!=UMI_STATUS_OK)return status;
    (void)index;
    if(workspace->revision==UINT64_MAX)return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingRegistry *registry=umi_chart_workspace_drawings(workspace->charts);char id[128];
    status=NextChartDrawingId(registry,id);
    if(status!=UMI_STATUS_OK)return status;
    if(strlen(id)+1U>capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    status=UmiChartDrawingDuplicate(registry,instrumentId,drawingId,expectedRevision,id);
    if(status==UMI_STATUS_OK){workspace->revision++;strcpy(outId,id);}
    return status;
}
#endif
UmiStatus UmiTradingWorkspaceDuplicateChartDrawing(UmiTradingWorkspace *workspace,
    const char *instrumentId,const char *drawingId,uint64_t expectedRevision,char *outId,size_t capacity)
{
    if (outId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    ChartHistoryRequest request = {.action=CH_DUPLICATE, .pane=instrumentId, .id=drawingId,
        .expected=expectedRevision, .output_capacity=capacity};
    UmiStatus status = ChartHistoryApply(workspace, &request, "Duplicate drawing");
    if (status == UMI_STATUS_OK) strcpy(outId, request.new_id);
    return status;
}

/* Canonical drawing edits now publish their reversible evidence with the registry, preserving atomic ownership in Framework. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiTradingWorkspaceRemoveChartDrawing(UmiTradingWorkspace *workspace,
    const char *instrument_id, const char *drawing_id, uint64_t expected_revision)
{
    if (drawing_id == NULL || drawing_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    UmiStatus status = ChartSelectedIndex(workspace, instrument_id, &index);
    if (status != UMI_STATUS_OK) return status;
    (void)index;
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(workspace->charts);
    UmiChartDrawingSnapshot drawing;
    status = umi_chart_drawing_registry_find(registry, drawing_id, &drawing);
    if (status != UMI_STATUS_OK) return status;
    if (drawing.locked || drawing.revision != expected_revision ||
        strcmp(drawing.pane_id, instrument_id) != 0 || workspace->revision == UINT64_MAX ||
        umi_chart_drawing_registry_revision(registry) == UINT64_MAX)
        return UMI_STATUS_INVALID_STATE;
    status = umi_chart_drawing_registry_remove(registry, drawing_id);
    if (status == UMI_STATUS_OK) workspace->revision++;
    return status;
}
#endif
UmiStatus UmiTradingWorkspaceRemoveChartDrawing(UmiTradingWorkspace *workspace,
    const char *instrument_id, const char *drawing_id, uint64_t expected_revision)
{
    if (drawing_id == NULL || drawing_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    ChartHistoryRequest request = {.action=CH_REMOVE, .pane=instrument_id, .id=drawing_id, .expected=expected_revision};
    return ChartHistoryApply(workspace, &request, "Remove drawing");
}
UmiStatus UmiTradingWorkspacePrepareChartLimit(UmiTradingWorkspace *workspace,
    const char *instrument_id, UmiSide side, double price)
{
    if ((side != UMI_SIDE_BUY && side != UMI_SIDE_SELL) || !isfinite(price) || price <= 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index;
    UmiStatus status = ChartSelectedIndex(workspace, instrument_id, &index);
    if (status != UMI_STATUS_OK) return status;
    (void)index;
    if (workspace->revision == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
    UmiOrderRequest draft = workspace->draft_order;
    if (strcmp(draft.instrument.instrument_id.value, instrument_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    draft.type = UMI_ORDER_LIMIT;
    draft.side = side;
    draft.limit_price = price;
    draft.stop_price = 0.0;
    workspace->draft_order = draft;
    workspace->has_draft_risk = 0;
    memset(&workspace->riskEvidence, 0, sizeof workspace->riskEvidence);
    workspace->revision++;
    return UMI_STATUS_OK;
}


UmiStatus UmiTradingWorkspaceOrderAt(const UmiTradingWorkspace *workspace,
    size_t index, UmiOrder *out_order)
{
    if (workspace == NULL || out_order == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= workspace->oms.orders.count) return UMI_STATUS_NOT_FOUND;
    *out_order = workspace->oms.orders.orders[index];
    return UMI_STATUS_OK;
}

/* Reporting must not call the general snapshot's selection reconciliation.
 * The existing order-visible predicate remains the single filtering rule;
 * this const capture copies matching records in the same newest-first order. */
#include "order_report_private.h"
UmiStatus UmiTradingCopyOrderReport(const UmiTradingWorkspace *workspace,
    UmiTradingOrderReport *outReport)
{
    if (workspace == NULL || outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->oms.orders.count > UMI_TRADING_MAX_ORDERS) return UMI_STATUS_INVALID_STATE;
    memset(outReport, 0, sizeof(*outReport));
    outReport->query.status = workspace->order_filter;
    memcpy(outReport->query.text, workspace->order_search, sizeof(outReport->query.text));
    outReport->revision = workspace->revision;
    outReport->retained = workspace->oms.orders.count;
    for (size_t i = workspace->oms.orders.count; i > 0U; --i) {
        const UmiOrder *order = &workspace->oms.orders.orders[i - 1U];
        if (order_visible(workspace, order)) outReport->orders[outReport->matching++] = *order;
    }
    return UMI_STATUS_OK;
}

/* Persistence captures the canonical chart model, independently of the GTK
 * lifetime and without the general workspace snapshot's selection repair. */
#include "umicom/trading/chart_document.h"
UmiStatus UmiTradingWorkspaceCaptureChart(const UmiTradingWorkspace *workspace,
    const char *instrumentId, UmiChartDocument **outDocument)
{
    if (outDocument != NULL) *outDocument = NULL;
    if (workspace == NULL || outDocument == NULL || instrumentId == NULL || instrumentId[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = market_index(workspace, instrumentId);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return UmiChartDocumentCapture(umi_chart_workspace_drawings(workspace->charts),
        instrumentId, &workspace->chart_navigation[index], outDocument);
}
/* The same tool semantics now govern gestures and restored charts. Supported boxes and rays retain their geometry and locks through the existing versioned checkpoint format; unknown tools still refuse restoration. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiTradingChartDocumentValidate(const UmiChartDocument *document)
{
    UmiChartDocumentSummary summary;
    UmiStatus status = UmiChartDocumentGetSummary(document, &summary);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < summary.drawing_count; ++i) {
        UmiChartDrawingSnapshot drawing;
        status = UmiChartDocumentDrawingAt(document, i, &drawing);
        if (status != UMI_STATUS_OK) return status;
        int trend = strcmp(drawing.tool, "trend") == 0;
        if (!trend && strcmp(drawing.tool, "support") != 0 && strcmp(drawing.tool, "resistance") != 0)
            return UMI_STATUS_UNAVAILABLE;
        if (drawing.time1 < 0 || drawing.time2 < 0 ||
            (trend && drawing.time1 == drawing.time2) || (!trend && drawing.value1 != drawing.value2))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiTradingChartDocumentValidate(const UmiChartDocument *document)
{
    UmiChartDocumentSummary summary;UmiStatus status=UmiChartDocumentGetSummary(document,&summary);
    if(status!=UMI_STATUS_OK)return status;
    for(size_t i=0;i<summary.drawing_count;++i){
        UmiChartDrawingSnapshot drawing;status=UmiChartDocumentDrawingAt(document,i,&drawing);
        if(status==UMI_STATUS_OK)status=UmiChartDrawingToolValidate(&drawing);
        if(status!=UMI_STATUS_OK)return status;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiTradingWorkspaceRestoreChart(UmiTradingWorkspace *workspace,
    const UmiChartDocument *document, uint64_t expectedDrawingRevision,
    const UmiChartNavigation *expectedNavigation)
{
    if (workspace == NULL || expectedNavigation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiTradingChartDocumentValidate(document);
    if (status != UMI_STATUS_OK) return status;
    UmiChartDocumentSummary summary;
    status = UmiChartDocumentGetSummary(document, &summary);
    if (status != UMI_STATUS_OK) return status;
    size_t index = market_index(workspace, summary.pane_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const UmiChartNavigation *current = &workspace->chart_navigation[index];
/* A saved-chart preview must reject restore after its timeframe changes, as it already rejects changed zoom and anchoring. The previous implementation remains for engineering review. */
#if 0
    if (current->visible_bars != expectedNavigation->visible_bars ||
        current->anchor_ms != expectedNavigation->anchor_ms || current->pinned != expectedNavigation->pinned)
        return UMI_STATUS_INVALID_STATE;
#endif
    if (current->visible_bars != expectedNavigation->visible_bars ||
        current->anchor_ms != expectedNavigation->anchor_ms || current->pinned != expectedNavigation->pinned ||
        current->interval_ms != expectedNavigation->interval_ms)
        return UMI_STATUS_INVALID_STATE;
    if (workspace->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Restore changes the complete view as well as drawings. Preflight the
     * history barrier before publication; Reset cannot fail on this owner thread
     * after this check, and failed document restores retain prior history. */
    if (workspace->drawing_history != NULL) {
        UmiChartDrawingHistorySnapshot history;
        status = UmiChartDrawingHistoryRead(workspace->drawing_history, &history);
        if (status != UMI_STATUS_OK) return status;
        if (history.revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = UmiChartDocumentApplyDrawings(document, umi_chart_workspace_drawings(workspace->charts), expectedDrawingRevision);
    if (status != UMI_STATUS_OK) return status;
    /* All validation and allocation completed before the single registry commit.
     * Publishing the copied view cannot fail, so drawings and view move together. */
    workspace->chart_navigation[index] = summary.navigation;
    if (workspace->drawing_history != NULL) (void)UmiChartDrawingHistoryReset(workspace->drawing_history);
    ++workspace->revision;
    return UMI_STATUS_OK;
}


/* Session reports capture one owner revision without touching UI selection,
 * applying order filters or acquiring any broker-routing capability. */
#include "session_report_private.h"
UmiStatus UmiTradingCopySessionSource(const UmiTradingWorkspace *workspace,
    UmiTradingSessionSource *outSource)
{
    if (workspace == NULL || outSource == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->oms.orders.count > UMI_TRADING_MAX_ORDERS ||
        workspace->executions.count > UMI_TRADING_MAX_ORDERS ||
        workspace->positions.count > UMI_TRADING_MAX_POSITIONS) return UMI_STATUS_INVALID_STATE;
    memset(outSource, 0, sizeof(*outSource));
    outSource->account = workspace->account_id;
    outSource->ownerIdentity = workspace->session_report_owner_id;
    outSource->environment = workspace->environment; outSource->revision = workspace->revision;
    outSource->orderCount = workspace->oms.orders.count;
    outSource->executionCount = workspace->executions.count;
    outSource->positionCount = workspace->positions.count;
    memcpy(outSource->orders, workspace->oms.orders.orders, outSource->orderCount * sizeof(UmiOrder));
    memcpy(outSource->executions, workspace->executions.reports, outSource->executionCount * sizeof(UmiExecutionReport));
    memcpy(outSource->positions, workspace->positions.positions, outSource->positionCount * sizeof(UmiPosition));
    return UMI_STATUS_OK;
}
UmiStatus UmiTradingSessionSourceCurrent(const UmiTradingWorkspace *workspace,
    const UmiTradingSessionSource *source, bool *outCurrent)
{
    if (outCurrent != NULL) *outCurrent = false;
    if (workspace == NULL || source == NULL || outCurrent == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outCurrent = workspace->session_report_owner_id == source->ownerIdentity &&
        workspace->revision == source->revision && workspace->environment == source->environment &&
        strcmp(workspace->account_id.value, source->account.value) == 0;
    return UMI_STATUS_OK;
}


#include "umicom/chart/drawing_visibility.h"
/* Drawing presentation uses the existing selected-instrument guard and chart
 * owner. No trading command or risk-preview invalidation is dispatched here. */
/* Canonical drawing edits now publish their reversible evidence with the registry, preserving atomic ownership in Framework. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiTradingWorkspaceSetChartDrawingHidden(UmiTradingWorkspace *workspace,
    const char *instrumentId, const char *drawingId, uint64_t expectedRevision, int hidden)
{
    size_t index;
    UmiStatus status = ChartSelectedIndex(workspace, instrumentId, &index);
    if (status != UMI_STATUS_OK) return status;
    (void)index;
    if (workspace->revision == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(workspace->charts);
    uint64_t before = umi_chart_drawing_registry_revision(registry);
    status = UmiChartDrawingSetHidden(registry, instrumentId, drawingId, expectedRevision, hidden);
    if (status == UMI_STATUS_OK && before != umi_chart_drawing_registry_revision(registry)) ++workspace->revision;
    return status;
}
#endif
UmiStatus UmiTradingWorkspaceSetChartDrawingHidden(UmiTradingWorkspace *workspace,
    const char *instrumentId, const char *drawingId, uint64_t expectedRevision, int hidden)
{
    ChartHistoryRequest request = {.action=CH_HIDE, .flag=hidden, .pane=instrumentId, .id=drawingId, .expected=expectedRevision};
    return ChartHistoryApply(workspace, &request, "Change visibility");
}

/* One explicit pane action publishes the batch through the shared registry. */
/* Canonical drawing edits now publish their reversible evidence with the registry, preserving atomic ownership in Framework. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiTradingWorkspaceSetChartDrawingsHidden(UmiTradingWorkspace *workspace,
    const char *instrumentId, uint64_t expectedRegistryRevision, int hidden, size_t *outChanged)
{
    size_t index;
    UmiStatus status = ChartSelectedIndex(workspace, instrumentId, &index);
    if (status != UMI_STATUS_OK) return status;
    (void)index;
    if (workspace->revision == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(workspace->charts);
    uint64_t before = umi_chart_drawing_registry_revision(registry);
    status = UmiChartDrawingSetPaneHidden(registry, instrumentId, expectedRegistryRevision, hidden, outChanged);
    if (status == UMI_STATUS_OK && before != umi_chart_drawing_registry_revision(registry)) ++workspace->revision;
    return status;
}
#endif
UmiStatus UmiTradingWorkspaceSetChartDrawingsHidden(UmiTradingWorkspace *workspace,
    const char *instrumentId, uint64_t expectedRegistryRevision, int hidden, size_t *outChanged)
{
    ChartHistoryRequest request = {.action=CH_HIDE_PANE, .pane=instrumentId, .expected=expectedRegistryRevision, .flag=hidden};
    UmiStatus status = ChartHistoryApply(workspace, &request, hidden ? "Hide all drawings" : "Show all drawings");
    if (status == UMI_STATUS_OK && outChanged != NULL) *outChanged = request.changed;
    return status;
}


/* Reuse the canonical drawing owner rather than placing presentation state in
 * Trader. Selected-instrument and revision guards prevent a delayed editor
 * from targeting a different instrument or overwriting a more recent edit. */
/* Canonical drawing edits now publish their reversible evidence with the registry, preserving atomic ownership in Framework. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiTradingWorkspaceSetChartDrawingAppearance(UmiTradingWorkspace *workspace,
    const char *instrumentId, const char *drawingId, uint64_t expectedRevision,
    const UmiChartDrawingAppearance *appearance)
{
    size_t index;
    UmiStatus status = ChartSelectedIndex(workspace, instrumentId, &index);
    if (status != UMI_STATUS_OK) return status;
    (void)index;
    if (workspace->revision == UINT64_MAX) return UMI_STATUS_INVALID_STATE;
    UmiChartDrawingRegistry *registry = umi_chart_workspace_drawings(workspace->charts);
    uint64_t before = umi_chart_drawing_registry_revision(registry);
    status = UmiChartDrawingSetAppearance(registry, instrumentId, drawingId, expectedRevision, appearance);
    if (status == UMI_STATUS_OK && before != umi_chart_drawing_registry_revision(registry)) ++workspace->revision;
    return status;
}
#endif
UmiStatus UmiTradingWorkspaceSetChartDrawingAppearance(UmiTradingWorkspace *workspace,
    const char *instrumentId, const char *drawingId, uint64_t expectedRevision,
    const UmiChartDrawingAppearance *appearance)
{
    ChartHistoryRequest request = {.action=CH_APPEARANCE, .appearance=appearance, .pane=instrumentId, .id=drawingId, .expected=expectedRevision};
    return ChartHistoryApply(workspace, &request, "Change appearance");
}
