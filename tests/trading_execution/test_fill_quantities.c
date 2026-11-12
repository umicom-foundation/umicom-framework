/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_fill_quantities.c
 *
 * PURPOSE:
 *   Verify represented quantity bounds, partial completion and workspace publication without live trading.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

static UmiExecutionReport Report(const UmiOrder *order, double quantity)
{
    UmiExecutionReport report = {0};
    strcpy(report.execution_id.value, "quantity-fill");
    report.client_order_id = order->request.client_order_id;
    report.fill_quantity = quantity; report.fill_price = 100.0;
    report.event_time_ms = 1000;
    return report;
}
static int Quantity(const char *name)
{
    UmiOrder order = {0};
    order.request = test_order_request(); order.request.quantity = 1.0;
    order.status = UMI_ORDER_ACCEPTED; order.version = 1U;
    UmiExecutionReport report = Report(&order, 0.5);
    UmiStatus expected = UMI_STATUS_OK;
    UmiOrderStatus expectedState = UMI_ORDER_PARTIALLY_FILLED;
    if (!strcmp(name, "tiny-partial") || !strcmp(name, "tiny-complete") || !strcmp(name, "tiny-overfill")) {
        order.request.quantity = 0x1p-40;
        report.fill_quantity = !strcmp(name, "tiny-partial") ? 0x1p-41 :
            (!strcmp(name, "tiny-complete") ? 0x1p-40 : 0x1p-39);
        if (!strcmp(name, "tiny-complete")) expectedState = UMI_ORDER_FILLED;
        if (!strcmp(name, "tiny-overfill")) expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "near-complete")) report.fill_quantity = nextafter(1.0, 0.0);
    else if (!strcmp(name, "near-overfill")) {
        report.fill_quantity = nextafter(1.0, INFINITY); expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "remaining-overfill")) {
        order.status = UMI_ORDER_PARTIALLY_FILLED;
        order.filled_quantity = 0.5; order.average_fill_price = 100.0;
        report.fill_quantity = nextafter(0.5, INFINITY); expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "lost-increment")) {
        order.request.quantity = 0x1p100; order.filled_quantity = 0x1p99;
        order.average_fill_price = 100.0; order.status = UMI_ORDER_PARTIALLY_FILLED;
        report.fill_quantity = 1.0; expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(name, "large-complete")) {
        order.request.quantity = DBL_MAX; order.filled_quantity = DBL_MAX / 2.0;
        order.average_fill_price = 100.0; order.status = UMI_ORDER_PARTIALLY_FILLED;
        report.fill_quantity = DBL_MAX / 2.0; expectedState = UMI_ORDER_FILLED;
    } else if (!strcmp(name, "subnormal-complete")) {
        order.request.quantity = DBL_TRUE_MIN; report.fill_quantity = DBL_TRUE_MIN;
        expectedState = UMI_ORDER_FILLED;
    } else if (!strcmp(name, "already-full") || !strcmp(name, "beyond-full")) {
        order.filled_quantity = !strcmp(name, "already-full") ? 1.0 : 2.0;
        order.average_fill_price = 100.0; order.status = UMI_ORDER_PARTIALLY_FILLED;
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "nan-stored")) {
        order.filled_quantity = NAN; expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "infinite-report")) {
        report.fill_quantity = INFINITY; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(name, "zero-report")) {
        report.fill_quantity = 0.0; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(name, "negative-report")) {
        report.fill_quantity = -1.0; expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(name, "version-exhausted")) {
        order.version = UINT64_MAX; expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "cancelled")) {
        order.status = UMI_ORDER_CANCELLED; expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(name, "two-step")) {
        report.fill_quantity = nextafter(1.0, 0.0);
    } else return 2;
    UmiOrder before;
    memcpy(&before, &order, sizeof(before));
    CHECK(umi_order_apply_execution(&order, &report) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(!memcmp(&order, &before, sizeof(order))); return 0;
    }
    CHECK(order.filled_quantity > before.filled_quantity);
    CHECK(order.filled_quantity <= order.request.quantity);
    CHECK(order.version == before.version + 1U);
    CHECK(order.status == expectedState && order.average_fill_price == 100.0);
    if (!strcmp(name, "two-step")) {
        strcpy(report.execution_id.value, "last-fill");
        report.fill_quantity = order.request.quantity - order.filled_quantity;
        CHECK(report.fill_quantity > 0.0);
        CHECK(umi_order_apply_execution(&order, &report) == UMI_STATUS_OK);
        CHECK(order.status == UMI_ORDER_FILLED && order.filled_quantity == 1.0);
        CHECK(order.version == 3U);
    }
    return 0;
}
static int Workspace(const char *name)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    UmiInstrument instrument = test_instrument();
    config.risk_limit = (UmiRiskLimit){1000.0, 1.0e12, 5000.0, 1.0e12};
    instrument.multiplier = 1.0;
    UmiRiskDecision decision;
    UmiTradingWorkspaceSnapshot before, after;
    UmiOrder order;
    UmiPosition position;
    CHECK(umi_trading_workspace_create(&config, &workspace) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_health(workspace, 1, 0, 1) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_quantity(workspace, 0x1p-40) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_prices(workspace, 100.0, 0.0) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_submit_order(workspace, 1000, &decision) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_visible_order_at(workspace, 0U, &order) == UMI_STATUS_OK);
    UmiExecutionReport report = Report(&order, 0x1p-41);
    CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_visible_order_at(workspace, 0U, &order) == UMI_STATUS_OK);
    CHECK(order.status == UMI_ORDER_PARTIALLY_FILLED && order.filled_quantity == 0x1p-41);
    CHECK(umi_trading_workspace_position_at(workspace, 0U, &position) == UMI_STATUS_OK);
    CHECK(position.quantity == 0x1p-41);
    CHECK(umi_trading_workspace_snapshot(workspace, &before) == UMI_STATUS_OK);
    if (!strcmp(name, "workspace-overfill")) {
        strcpy(report.execution_id.value, "too-much");
        report.fill_quantity = 0x1p-40;
        CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (!strcmp(name, "workspace-replay")) {
        CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_OK);
        report.fill_price = 101.0;
        CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_ALREADY_EXISTS);
    } else if (!strcmp(name, "workspace-cancel")) {
        CHECK(umi_trading_workspace_cancel_selected_order(workspace) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_snapshot(workspace, &before) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_visible_order_at(workspace, 0U, &order) == UMI_STATUS_OK);
        CHECK(order.status == UMI_ORDER_CANCELLED && order.filled_quantity == 0x1p-41);
        CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_OK);
        strcpy(report.execution_id.value, "after-cancel");
        CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_INVALID_STATE);
    } else if (!strcmp(name, "workspace-partial")) {
        strcpy(report.execution_id.value, "second-half");
        CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_OK);
        CHECK(umi_trading_workspace_visible_order_at(workspace, 0U, &order) == UMI_STATUS_OK);
        CHECK(order.status == UMI_ORDER_FILLED && order.filled_quantity == 0x1p-40);
        CHECK(umi_trading_workspace_position_at(workspace, 0U, &position) == UMI_STATUS_OK);
        CHECK(position.quantity == 0x1p-40);
        umi_trading_workspace_destroy(workspace); return 0;
    } else { umi_trading_workspace_destroy(workspace); return 2; }
    CHECK(umi_trading_workspace_snapshot(workspace, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && after.execution_count == before.execution_count);
    CHECK(after.position_count == before.position_count);
    CHECK(umi_trading_workspace_position_at(workspace, 0U, &position) == UMI_STATUS_OK);
    CHECK(position.quantity == 0x1p-41);
    umi_trading_workspace_destroy(workspace); return 0;
}

static int Position(const char *name)
{
    if (strcmp(name, "position-long-increase") && strcmp(name, "position-long-reduce") &&
        strcmp(name, "position-short-increase") && strcmp(name, "position-short-reduce")) return 2;
    UmiPosition position = {0};
    position.instrument = test_instrument(); position.instrument.multiplier = 1.0;
    position.quantity = !strcmp(name, "position-short-increase") || !strcmp(name, "position-short-reduce")
        ? -0x1p100 : 0x1p100;
    position.average_price = 100.0; position.realised_pnl = 7.0;
    UmiSide side = !strcmp(name, "position-short-increase") || !strcmp(name, "position-long-reduce")
        ? UMI_SIDE_SELL : UMI_SIDE_BUY;
    UmiPosition before;
    memcpy(&before, &position, sizeof(before));
    CHECK(umi_position_apply_fill(&position, side, 1.0, 101.0) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(!memcmp(&before, &position, sizeof(position)));
    /* A representable tiny close remains supported and resets its average. */
    position.quantity = 0x1p-40; position.average_price = 100.0; position.realised_pnl = 0.0;
    CHECK(umi_position_apply_fill(&position, UMI_SIDE_SELL, 0x1p-40, 101.0) == UMI_STATUS_OK);
    CHECK(position.quantity == 0.0 && position.average_price == 0.0);
    CHECK(position.realised_pnl == 0x1p-40);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strncmp(argv[1], "position-", 9U)) return Position(argv[1]);
    return !strncmp(argv[1], "workspace-", 10U) ? Workspace(argv[1]) : Quantity(argv[1]);
}
