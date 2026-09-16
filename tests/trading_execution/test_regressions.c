/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_regressions.c
 * PURPOSE: Reproduce execution defects through existing public entry points.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"

/* Checks remain active in Release; no assert controls test execution. */
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; \
} } while (0)

static UmiExecutionReport Report(const char *orderId)
{
    UmiExecutionReport report = {0};
    (void)snprintf(report.execution_id.value, sizeof report.execution_id.value, "fill-1");
    (void)snprintf(report.client_order_id.value, sizeof report.client_order_id.value, "%s", orderId);
    report.fill_quantity = 1.0; report.fill_price = 25020.0; report.event_time_ms = 2000;
    return report;
}

static int UnpricedMarket(void)
{
    UmiOrderRequest request = test_order_request();
    UmiRiskLimit limits = {10, 100, 10, 1000};
    request.type = UMI_ORDER_MARKET; request.limit_price = 0.0;
    UmiRiskDecision decision = umi_pretrade_risk_evaluate(&request, &limits, 0, 0);
    printf("unpriced market allowed=%d reason=%s\n", decision.allowed, decision.reason);
    CHECK(!decision.allowed); return 0;
}

static int DuplicateExecution(void)
{
    UmiExecutionStore store = {0}; UmiExecutionReport report = Report("ORDER-1");
    CHECK(umi_execution_store_add(&store, &report) == UMI_STATUS_OK);
    UmiStatus result = umi_execution_store_add(&store, &report);
    printf("duplicate execution status=%d count=%zu\n", result, store.count);
    CHECK(result == UMI_STATUS_ALREADY_EXISTS); CHECK(store.count == 1U); return 0;
}

static int RealisedMultiplier(void)
{
    UmiPosition position = {0}; position.instrument = test_instrument();
    CHECK(umi_position_apply_fill(&position, UMI_SIDE_BUY, 2, 25000) == UMI_STATUS_OK);
    CHECK(umi_position_apply_fill(&position, UMI_SIDE_SELL, 1, 25020) == UMI_STATUS_OK);
    printf("realised=%g expected=400; remaining unrealised=%g\n", position.realised_pnl,
        umi_position_unrealised_pnl(&position, 25020));
    CHECK(position.realised_pnl == 400.0); return 0;
}

static int InvalidFill(void)
{
    UmiPosition position = {0}; position.instrument = test_instrument();
    UmiPosition before = position;
    UmiStatus status = umi_position_apply_fill(&position, UMI_SIDE_BUY, NAN, 100);
    printf("NaN fill status=%d quantity=%g\n", status, position.quantity);
    CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&position, &before, sizeof position) == 0); return 0;
}

static int CancelledFill(void)
{
    UmiOrder order = {0}; order.request = test_order_request();
    order.status = UMI_ORDER_CANCELLED; order.version = 1;
    UmiOrder before = order; UmiExecutionReport report = Report("ORDER-1");
    UmiStatus status = umi_order_apply_execution(&order, &report);
    printf("cancelled-order fill status=%d order-status=%d\n", status, order.status);
    CHECK(status == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&before, &order, sizeof order) == 0); return 0;
}

static int DuplicateOrder(void)
{
    UmiOms *oms = calloc(1, sizeof *oms); CHECK(oms != NULL);
    UmiRiskLimit limits = {10, 1000000, 20, 1000};
    UmiOrderRequest request = test_order_request(); UmiRiskDecision decision = {0};
    umi_oms_init(oms, limits);
    CHECK(umi_oms_submit(oms, &request, 0, 0, 1000, &decision) == UMI_STATUS_OK);
    UmiStatus status = umi_oms_submit(oms, &request, 0, 0, 1001, &decision);
    printf("duplicate order status=%d allowed=%d count=%zu\n", status, decision.allowed, oms->orders.count);
    int ok = status == UMI_STATUS_ALREADY_EXISTS && !decision.allowed && oms->orders.count == 1U;
    free(oms); CHECK(ok); return 0;
}

static int QuoteRollback(void)
{
    UmiTradingWorkspace *workspace = NULL; UmiInstrument instrument = test_instrument();
    UmiQuote quote = {0}; UmiTradingMarketSnapshot market;
    CHECK(umi_trading_workspace_create(NULL, &workspace) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
    quote.instrument = instrument; quote.bid = 100; quote.ask = 101;
    quote.bid_size = 10; quote.ask_size = 10; quote.event_time_ms = 2000;
    CHECK(umi_trading_workspace_update_quote(workspace, &quote) == UMI_STATUS_OK);
    quote.event_time_ms = 1000; quote.bid = 50; quote.ask = 51;
    UmiStatus status = umi_trading_workspace_update_quote(workspace, &quote);
    CHECK(umi_trading_workspace_selected_market(workspace, &market) == UMI_STATUS_OK);
    printf("older quote status=%d retained-time=%lld\n", status, (long long)market.quote.event_time_ms);
    int ok = status == UMI_STATUS_INVALID_STATE && market.quote.event_time_ms == 2000;
    umi_trading_workspace_destroy(workspace); CHECK(ok); return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "unpriced-market") == 0) return UnpricedMarket();
    if (strcmp(argv[1], "execution-duplicate") == 0) return DuplicateExecution();
    if (strcmp(argv[1], "realised-multiplier") == 0) return RealisedMultiplier();
    if (strcmp(argv[1], "invalid-fill") == 0) return InvalidFill();
    if (strcmp(argv[1], "cancelled-fill") == 0) return CancelledFill();
    if (strcmp(argv[1], "order-duplicate") == 0) return DuplicateOrder();
    if (strcmp(argv[1], "quote-rollback") == 0) return QuoteRollback();
    return 2;
}
