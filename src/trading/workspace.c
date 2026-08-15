/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/workspace.c
 *
 * PURPOSE:
 *   Implement the professional trading workspace by composing the existing
 *   Framework watchlist, OMS, risk, execution, position and chart services.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
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

struct UmiTradingWorkspace {
    UmiFinancialId account_id;
    UmiTradingEnvironment environment;
    UmiWatchlist watchlist;
    UmiTradingMarketSnapshot markets[UMI_TRADING_MAX_WATCHLIST];
    size_t market_count;
    UmiOms oms;
    UmiExecutionStore executions;
    UmiPositionBook positions;
    UmiChartWorkspace *charts;
    UmiOrderRequest draft_order;
    UmiRiskDecision draft_risk;
    char instrument_filter[UMI_TRADING_WORKSPACE_FILTER_CAPACITY];
    UmiTradingWorkspaceOrderFilter order_filter;
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

static void copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U) return;
    if (source == NULL) source = "";
    length = strlen(source);
    if (length >= capacity) length = capacity - 1U;
    if (length > 0U) memcpy(destination, source, length);
    destination[length] = '\0';
}

static int valid_environment(UmiTradingEnvironment environment)
{
    return environment >= UMI_TRADING_SIMULATION &&
           environment <= UMI_TRADING_LIVE;
}

static int valid_order_filter(UmiTradingWorkspaceOrderFilter order_filter)
{
    return order_filter >= UMI_TRADING_WORKSPACE_ORDERS_ALL &&
           order_filter <= UMI_TRADING_WORKSPACE_ORDERS_REJECTED;
}

static int contains_case_insensitive(const char *text, const char *query)
{
    const unsigned char *candidate;
    size_t query_length;

    if (query == NULL || query[0] == '\0') return 1;
    if (text == NULL) return 0;
    query_length = strlen(query);
    for (candidate = (const unsigned char *)text;
         *candidate != '\0'; ++candidate) {
        size_t index;
        for (index = 0U; index < query_length; ++index) {
            unsigned char left = candidate[index];
            unsigned char right = (unsigned char)query[index];
            if (left == '\0' || tolower(left) != tolower(right)) break;
        }
        if (index == query_length) return 1;
    }
    return 0;
}

static size_t market_index(const UmiTradingWorkspace *workspace,
                           const char *instrument_id)
{
    size_t index;

    if (workspace == NULL || instrument_id == NULL) return SIZE_MAX;
    for (index = 0U; index < workspace->market_count; ++index) {
        if (strcmp(workspace->markets[index].instrument.instrument_id.value,
                   instrument_id) == 0) return index;
    }
    return SIZE_MAX;
}

static size_t order_index(const UmiTradingWorkspace *workspace,
                          const char *client_order_id)
{
    size_t index;

    if (workspace == NULL || client_order_id == NULL) return SIZE_MAX;
    for (index = 0U; index < workspace->oms.orders.count; ++index) {
        if (strcmp(workspace->oms.orders.orders[index]
                       .request.client_order_id.value,
                   client_order_id) == 0) return index;
    }
    return SIZE_MAX;
}

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

static int order_visible(const UmiTradingWorkspace *workspace,
                         const UmiOrder *order)
{
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

static size_t visible_market_count(const UmiTradingWorkspace *workspace)
{
    size_t index;
    size_t count = 0U;
    for (index = 0U; index < workspace->market_count; ++index)
        if (market_visible(workspace, &workspace->markets[index])) count += 1U;
    return count;
}

static size_t visible_order_count(const UmiTradingWorkspace *workspace)
{
    size_t index;
    size_t count = 0U;
    for (index = 0U; index < workspace->oms.orders.count; ++index)
        if (order_visible(workspace, &workspace->oms.orders.orders[index]))
            count += 1U;
    return count;
}

static double current_position_quantity(UmiTradingWorkspace *workspace)
{
    UmiPosition *position = NULL;
    if (workspace->draft_order.instrument.instrument_id.value[0] == '\0')
        return 0.0;
    if (umi_position_book_get(&workspace->positions,
                              &workspace->draft_order.instrument, 0,
                              &position) != UMI_STATUS_OK) return 0.0;
    return position->quantity;
}

static double realised_pnl(const UmiTradingWorkspace *workspace)
{
    size_t index;
    double total = 0.0;
    for (index = 0U; index < workspace->positions.count; ++index)
        total += workspace->positions.positions[index].realised_pnl;
    return total;
}

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

static void choose_instrument(UmiTradingWorkspace *workspace,
                              const UmiTradingMarketSnapshot *market)
{
    double reference_price = 0.0;
    copy_text(workspace->selected_instrument_id,
              sizeof(workspace->selected_instrument_id),
              market->instrument.instrument_id.value);
    workspace->draft_order.instrument = market->instrument;
    if (market->has_quote) reference_price = umi_quote_mid(&market->quote);
    else if (market->has_bar) reference_price = market->bar.close;
    workspace->draft_order.limit_price = reference_price;
    workspace->draft_order.stop_price = 0.0;
    workspace->has_draft_risk = 0;
}

static void reconcile_selections(UmiTradingWorkspace *workspace)
{
    size_t index = market_index(workspace, workspace->selected_instrument_id);
    if (index == SIZE_MAX || !market_visible(workspace,
                                             &workspace->markets[index])) {
        workspace->selected_instrument_id[0] = '\0';
        for (index = 0U; index < workspace->market_count; ++index) {
            if (market_visible(workspace, &workspace->markets[index])) {
                choose_instrument(workspace, &workspace->markets[index]);
                break;
            }
        }
    }

    index = order_index(workspace, workspace->selected_order_id);
    if (index == SIZE_MAX ||
        !order_visible(workspace, &workspace->oms.orders.orders[index])) {
        workspace->selected_order_id[0] = '\0';
        for (index = workspace->oms.orders.count; index > 0U; --index) {
            UmiOrder *order = &workspace->oms.orders.orders[index - 1U];
            if (order_visible(workspace, order)) {
                copy_text(workspace->selected_order_id,
                          sizeof(workspace->selected_order_id),
                          order->request.client_order_id.value);
                break;
            }
        }
    }
}

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

UmiStatus umi_trading_workspace_create(
    const UmiTradingWorkspaceConfig *config,
    UmiTradingWorkspace **out_workspace)
{
    UmiTradingWorkspaceConfig effective;
    UmiTradingWorkspace *workspace;
    UmiStatus status;

    if (out_workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_workspace = NULL;
    effective = config != NULL
        ? *config : umi_trading_workspace_config_default();
    if (effective.structure_size < sizeof(effective) ||
        effective.api_version != UMI_TRADING_WORKSPACE_API_VERSION ||
        !umi_financial_id_valid(&effective.account_id) ||
        !umi_risk_limit_valid(&effective.risk_limit) ||
        !valid_environment(effective.environment)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    workspace = (UmiTradingWorkspace *)calloc(1U, sizeof(*workspace));
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
    initialise_draft(workspace);
    status = umi_chart_workspace_create(&workspace->charts);
    if (status != UMI_STATUS_OK) {
        umi_trading_workspace_destroy(workspace);
        return status;
    }
    *out_workspace = workspace;
    return UMI_STATUS_OK;
}

void umi_trading_workspace_destroy(UmiTradingWorkspace *workspace)
{
    if (workspace == NULL) return;
    umi_chart_workspace_destroy(workspace->charts);
    free(workspace);
}

UmiStatus umi_trading_workspace_add_instrument(
    UmiTradingWorkspace *workspace,
    const UmiInstrument *instrument)
{
    UmiTradingMarketSnapshot *market;
    UmiStatus status;

    if (workspace == NULL || instrument == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->market_count >= UMI_TRADING_MAX_WATCHLIST)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_watchlist_add(&workspace->watchlist, instrument);
    if (status != UMI_STATUS_OK) return status;
    market = &workspace->markets[workspace->market_count++];
    memset(market, 0, sizeof(*market));
    market->structure_size = (uint32_t)sizeof(*market);
    market->api_version = UMI_TRADING_WORKSPACE_API_VERSION;
    market->instrument = *instrument;
    market->market_state = UMI_MARKET_CLOSED;
    market->revision = 1U;
    if (workspace->selected_instrument_id[0] == '\0')
        choose_instrument(workspace, market);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_remove_instrument(
    UmiTradingWorkspace *workspace,
    const char *instrument_id)
{
    size_t index;

    if (workspace == NULL || instrument_id == NULL || instrument_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, instrument_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (index + 1U < workspace->market_count) {
        memmove(&workspace->markets[index], &workspace->markets[index + 1U],
                (workspace->market_count - index - 1U) *
                    sizeof(workspace->markets[0]));
        memmove(&workspace->watchlist.instruments[index],
                &workspace->watchlist.instruments[index + 1U],
                (workspace->watchlist.count - index - 1U) *
                    sizeof(workspace->watchlist.instruments[0]));
    }
    workspace->market_count -= 1U;
    workspace->watchlist.count -= 1U;
    workspace->revision += 1U;
    reconcile_selections(workspace);
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_update_quote(
    UmiTradingWorkspace *workspace,
    const UmiQuote *quote)
{
    size_t index;
    if (workspace == NULL || quote == NULL || !umi_quote_valid(quote))
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, quote->instrument.instrument_id.value);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    workspace->markets[index].quote = *quote;
    workspace->markets[index].has_quote = 1;
    workspace->markets[index].revision += 1U;
    workspace->market_data_ready = 1;
    if (strcmp(workspace->selected_instrument_id,
               quote->instrument.instrument_id.value) == 0 &&
        workspace->draft_order.limit_price <= 0.0) {
        workspace->draft_order.limit_price = umi_quote_mid(quote);
    }
    workspace->revision += 1U;
    workspace->has_draft_risk = 0;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_update_bar(
    UmiTradingWorkspace *workspace,
    const UmiBar *bar,
    double previous_close)
{
    size_t index;
    if (workspace == NULL || bar == NULL || bar->close <= 0.0 ||
        bar->high < bar->low || previous_close < 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, bar->instrument.instrument_id.value);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    workspace->markets[index].bar = *bar;
    workspace->markets[index].previous_close = previous_close;
    workspace->markets[index].has_bar = 1;
    workspace->markets[index].revision += 1U;
    workspace->market_data_ready = 1;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_update_depth(
    UmiTradingWorkspace *workspace,
    const UmiMarketDepth *depth)
{
    size_t index;
    if (workspace == NULL || depth == NULL || !umi_market_depth_valid(depth))
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, depth->instrument.instrument_id.value);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    workspace->markets[index].depth = *depth;
    workspace->markets[index].has_depth = 1;
    workspace->markets[index].revision += 1U;
    workspace->market_data_ready = 1;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_set_market_state(
    UmiTradingWorkspace *workspace,
    const char *instrument_id,
    UmiMarketState state)
{
    size_t index;
    UmiTradingMarketSnapshot *market;
    if (workspace == NULL || instrument_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, instrument_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    market = &workspace->markets[index];
    if (!umi_market_state_transition_allowed(market->market_state, state))
        return UMI_STATUS_INVALID_STATE;
    market->market_state = state;
    market->revision += 1U;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_set_instrument_filter(
    UmiTradingWorkspace *workspace,
    const char *filter_text)
{
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    copy_text(workspace->instrument_filter,
              sizeof(workspace->instrument_filter), filter_text);
    workspace->revision += 1U;
    reconcile_selections(workspace);
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_set_order_filter(
    UmiTradingWorkspace *workspace,
    UmiTradingWorkspaceOrderFilter order_filter)
{
    if (workspace == NULL || !valid_order_filter(order_filter))
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->order_filter = order_filter;
    workspace->revision += 1U;
    reconcile_selections(workspace);
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_select_instrument(
    UmiTradingWorkspace *workspace,
    const char *instrument_id)
{
    size_t index;
    if (workspace == NULL || instrument_id == NULL || instrument_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, instrument_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    choose_instrument(workspace, &workspace->markets[index]);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_select_order(
    UmiTradingWorkspace *workspace,
    const char *client_order_id)
{
    if (workspace == NULL || client_order_id == NULL ||
        client_order_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (order_index(workspace, client_order_id) == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    copy_text(workspace->selected_order_id,
              sizeof(workspace->selected_order_id), client_order_id);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_set_environment(
    UmiTradingWorkspace *workspace,
    UmiTradingEnvironment environment)
{
    if (workspace == NULL || !valid_environment(environment))
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->environment = environment;
    workspace->draft_order.environment = environment;
    if (environment != UMI_TRADING_LIVE) workspace->live_armed = 0;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_set_health(
    UmiTradingWorkspace *workspace,
    int market_data_ready,
    int broker_ready,
    int risk_ready)
{
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workspace->market_data_ready = market_data_ready != 0;
    workspace->broker_ready = broker_ready != 0;
    workspace->risk_ready = risk_ready != 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_set_live_armed(
    UmiTradingWorkspace *workspace,
    int armed)
{
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (armed && (workspace->environment != UMI_TRADING_LIVE ||
                  !workspace->broker_ready || !workspace->risk_ready))
        return UMI_STATUS_INVALID_STATE;
    workspace->live_armed = armed != 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_set_draft_side(
    UmiTradingWorkspace *workspace,
    UmiSide side)
{
    if (workspace == NULL || (side != UMI_SIDE_BUY && side != UMI_SIDE_SELL))
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->draft_order.side = side;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_set_draft_type(
    UmiTradingWorkspace *workspace,
    UmiOrderType type,
    UmiTimeInForce time_in_force)
{
    if (workspace == NULL || type < UMI_ORDER_MARKET ||
        type > UMI_ORDER_STOP_LIMIT ||
        !umi_time_in_force_valid(time_in_force))
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->draft_order.type = type;
    workspace->draft_order.tif = time_in_force;
    if (!umi_order_type_requires_stop(type))
        workspace->draft_order.stop_price = 0.0;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_set_draft_quantity(
    UmiTradingWorkspace *workspace,
    double quantity)
{
    if (workspace == NULL || quantity <= 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->draft_order.quantity = quantity;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_set_draft_prices(
    UmiTradingWorkspace *workspace,
    double limit_price,
    double stop_price)
{
    if (workspace == NULL || limit_price < 0.0 || stop_price < 0.0)
        return UMI_STATUS_INVALID_ARGUMENT;
    workspace->draft_order.limit_price = limit_price;
    workspace->draft_order.stop_price = stop_price;
    workspace->has_draft_risk = 0;
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_preview_order(
    UmiTradingWorkspace *workspace,
    UmiRiskDecision *out_decision)
{
    UmiStatus status;
    if (workspace == NULL || out_decision == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_order_request_validate(&workspace->draft_order);
    if (status != UMI_STATUS_OK) {
        umi_risk_decision_deny(&workspace->draft_risk,
                               "invalid order request");
    } else {
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

UmiStatus umi_trading_workspace_submit_order(
    UmiTradingWorkspace *workspace,
    int64_t now_ms,
    UmiRiskDecision *out_decision)
{
    UmiStatus status;
    int ready;

    if (workspace == NULL || out_decision == NULL || now_ms < 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    ready = umi_trading_health_ready(
        workspace->market_data_ready, workspace->broker_ready,
        workspace->risk_ready, workspace->environment);
    if (!ready) {
        umi_risk_decision_deny(out_decision,
                               "trading services are not ready");
        workspace->draft_risk = *out_decision;
        workspace->has_draft_risk = 1;
        return UMI_STATUS_UNAVAILABLE;
    }
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
    if (status == UMI_STATUS_OK) {
        copy_text(workspace->selected_order_id,
                  sizeof(workspace->selected_order_id),
                  workspace->draft_order.client_order_id.value);
    }
    return status;
}

UmiStatus umi_trading_workspace_cancel_selected_order(
    UmiTradingWorkspace *workspace)
{
    size_t index;
    UmiOrder *order;
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = order_index(workspace, workspace->selected_order_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    order = &workspace->oms.orders.orders[index];
    if (!umi_order_transition_allowed(order->status, UMI_ORDER_CANCELLED))
        return UMI_STATUS_INVALID_STATE;
    order->status = UMI_ORDER_CANCELLED;
    order->version += 1U;
    workspace->revision += 1U;
    reconcile_selections(workspace);
    return UMI_STATUS_OK;
}

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

    if (workspace == NULL || !umi_execution_report_valid(report))
        return UMI_STATUS_INVALID_ARGUMENT;
    index = order_index(workspace, report->client_order_id.value);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    order = &workspace->oms.orders.orders[index];
    if (order->status != UMI_ORDER_ACCEPTED &&
        order->status != UMI_ORDER_PARTIALLY_FILLED)
        return UMI_STATUS_INVALID_STATE;
    remaining = order->request.quantity - order->filled_quantity;
    if (report->fill_quantity > remaining) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_position_book_get(&workspace->positions,
                                   &order->request.instrument, 1, &position);
    if (status != UMI_STATUS_OK) return status;
    status = umi_execution_store_add(&workspace->executions, report);
    if (status != UMI_STATUS_OK) return status;
    status = umi_position_apply_fill(position, order->request.side,
                                     report->fill_quantity,
                                     report->fill_price);
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

void umi_trading_workspace_engage_kill_switch(
    UmiTradingWorkspace *workspace,
    const char *reason)
{
    if (workspace == NULL) return;
    umi_kill_switch_engage(&workspace->oms.kill_switch,
                           reason != NULL ? reason : "operator request");
    workspace->live_armed = 0;
    workspace->revision += 1U;
}

void umi_trading_workspace_reset_kill_switch(UmiTradingWorkspace *workspace)
{
    if (workspace == NULL) return;
    umi_kill_switch_reset(&workspace->oms.kill_switch);
    workspace->revision += 1U;
}

UmiStatus umi_trading_workspace_refresh(UmiTradingWorkspace *workspace)
{
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    reconcile_selections(workspace);
    workspace->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_snapshot(
    UmiTradingWorkspace *workspace,
    UmiTradingWorkspaceSnapshot *out_snapshot)
{
    UmiTradingMarketSnapshot market;
    size_t selected_order;
    size_t index;

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

    if (umi_trading_workspace_selected_market(workspace, &market) ==
        UMI_STATUS_OK) {
        out_snapshot->has_selected_instrument = 1;
        out_snapshot->has_quote = market.has_quote;
        out_snapshot->has_bar = market.has_bar;
        out_snapshot->has_depth = market.has_depth;
        if (market.has_quote) {
            out_snapshot->selected_bid = market.quote.bid;
            out_snapshot->selected_ask = market.quote.ask;
            out_snapshot->selected_mid = umi_quote_mid(&market.quote);
            out_snapshot->selected_spread = umi_quote_spread(&market.quote);
        }
        if (market.has_bar && market.previous_close > 0.0) {
            out_snapshot->selected_change =
                market.bar.close - market.previous_close;
            out_snapshot->selected_change_percent =
                out_snapshot->selected_change / market.previous_close * 100.0;
        }
        if (market.has_depth) {
            out_snapshot->selected_depth_imbalance =
                umi_order_book_imbalance(&market.depth);
            out_snapshot->selected_top_liquidity =
                umi_order_book_top_liquidity(&market.depth);
        }
    }
    selected_order = order_index(workspace, workspace->selected_order_id);
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
    for (index = 0U; index < workspace->positions.count; ++index)
        out_snapshot->revision +=
            (uint64_t)(workspace->positions.positions[index].quantity != 0.0);
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_visible_instrument_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiTradingMarketSnapshot *out_market)
{
    size_t source_index;
    size_t visible_index = 0U;
    if (workspace == NULL || out_market == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (source_index = 0U; source_index < workspace->market_count;
         ++source_index) {
        if (!market_visible(workspace, &workspace->markets[source_index]))
            continue;
        if (visible_index == index) {
            *out_market = workspace->markets[source_index];
            return UMI_STATUS_OK;
        }
        visible_index += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_trading_workspace_selected_market(
    UmiTradingWorkspace *workspace,
    UmiTradingMarketSnapshot *out_market)
{
    size_t index;
    if (workspace == NULL || out_market == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = market_index(workspace, workspace->selected_instrument_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_market = workspace->markets[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_visible_order_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiOrder *out_order)
{
    size_t source_index;
    size_t visible_index = 0U;
    if (workspace == NULL || out_order == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (source_index = workspace->oms.orders.count; source_index > 0U;
         --source_index) {
        UmiOrder *order = &workspace->oms.orders.orders[source_index - 1U];
        if (!order_visible(workspace, order)) continue;
        if (visible_index == index) {
            *out_order = *order;
            return UMI_STATUS_OK;
        }
        visible_index += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_trading_workspace_position_at(
    UmiTradingWorkspace *workspace,
    size_t index,
    UmiPosition *out_position)
{
    if (workspace == NULL || out_position == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= workspace->positions.count) return UMI_STATUS_NOT_FOUND;
    *out_position = workspace->positions.positions[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_trading_workspace_execution_at(
    UmiTradingWorkspace *workspace,
    size_t newest_first_index,
    UmiExecutionReport *out_report)
{
    if (workspace == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (newest_first_index >= workspace->executions.count)
        return UMI_STATUS_NOT_FOUND;
    *out_report = workspace->executions.reports[
        workspace->executions.count - newest_first_index - 1U];
    return UMI_STATUS_OK;
}

UmiChartWorkspace *umi_trading_workspace_charts(
    UmiTradingWorkspace *workspace)
{
    return workspace != NULL ? workspace->charts : NULL;
}

const char *umi_trading_environment_text(UmiTradingEnvironment environment)
{
    switch (environment) {
        case UMI_TRADING_PAPER: return "paper";
        case UMI_TRADING_LIVE: return "live";
        case UMI_TRADING_SIMULATION:
        default: return "simulation";
    }
}

const char *umi_trading_side_text(UmiSide side)
{
    return side == UMI_SIDE_SELL ? "sell" : "buy";
}

const char *umi_trading_order_type_text(UmiOrderType type)
{
    switch (type) {
        case UMI_ORDER_LIMIT: return "limit";
        case UMI_ORDER_STOP: return "stop";
        case UMI_ORDER_STOP_LIMIT: return "stop-limit";
        case UMI_ORDER_MARKET:
        default: return "market";
    }
}

const char *umi_trading_time_in_force_text(UmiTimeInForce time_in_force)
{
    switch (time_in_force) {
        case UMI_TIF_GTC: return "GTC";
        case UMI_TIF_IOC: return "IOC";
        case UMI_TIF_FOK: return "FOK";
        case UMI_TIF_DAY:
        default: return "DAY";
    }
}

const char *umi_trading_order_status_text(UmiOrderStatus status)
{
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

const char *umi_trading_market_state_text(UmiMarketState state)
{
    switch (state) {
        case UMI_MARKET_PREOPEN: return "pre-open";
        case UMI_MARKET_OPEN: return "open";
        case UMI_MARKET_HALTED: return "halted";
        case UMI_MARKET_CLOSED:
        default: return "closed";
    }
}

const char *umi_trading_workspace_order_filter_text(
    UmiTradingWorkspaceOrderFilter order_filter)
{
    switch (order_filter) {
        case UMI_TRADING_WORKSPACE_ORDERS_OPEN: return "open";
        case UMI_TRADING_WORKSPACE_ORDERS_FILLED: return "filled";
        case UMI_TRADING_WORKSPACE_ORDERS_CANCELLED: return "cancelled";
        case UMI_TRADING_WORKSPACE_ORDERS_REJECTED: return "rejected";
        case UMI_TRADING_WORKSPACE_ORDERS_ALL:
        default: return "all";
    }
}
