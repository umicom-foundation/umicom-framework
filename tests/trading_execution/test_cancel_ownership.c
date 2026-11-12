/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_cancel_ownership.c
 *
 * PURPOSE:
 *   Verify shared cancellation, retained partial fills and atomic admission of order/workspace revisions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
/* Compile the real workspace owner here so revision exhaustion can be injected
 * without a product setter or a copied private struct definition. The linked
 * trading archive supplies the remaining canonical owners; its workspace
 * object is not needed. All broker and workspace operations remain local. */
#include "../../src/trading/workspace.c"
#define CANCEL_CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

static int CancelValueCase(const char *name)
{
    UmiOrder order = {0};
    order.request = test_order_request(); order.request.quantity = 2.0;
    order.status = UMI_ORDER_ACCEPTED; order.version = 1U;
    UmiStatus expected = UMI_STATUS_OK;
    int changes = 1;
    if (!strcmp(name, "accepted")) {}
    else if (!strcmp(name, "partial")) {
        order.status = UMI_ORDER_PARTIALLY_FILLED; order.filled_quantity = 0.5; order.average_fill_price = 100.0;
    } else if (!strcmp(name, "cancelled") || !strcmp(name, "replay-at-max")) {
        order.status = UMI_ORDER_CANCELLED; order.filled_quantity = 0.5; order.average_fill_price = 100.0;
        if (!strcmp(name, "replay-at-max")) order.version = UINT64_MAX;
        changes = 0;
    } else if (!strcmp(name, "last-revision")) order.version = UINT64_MAX - 1U;
    else if (!strcmp(name, "exhausted")) { order.version = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(name, "new")) { order.status = UMI_ORDER_NEW; expected = UMI_STATUS_INVALID_STATE; }
    else if (!strcmp(name, "validated")) { order.status = UMI_ORDER_VALIDATED; expected = UMI_STATUS_INVALID_STATE; }
    else if (!strcmp(name, "filled")) { order.status = UMI_ORDER_FILLED; expected = UMI_STATUS_INVALID_STATE; }
    else if (!strcmp(name, "rejected")) { order.status = UMI_ORDER_REJECTED; expected = UMI_STATUS_INVALID_STATE; }
    else if (!strcmp(name, "unknown")) { order.status = (UmiOrderStatus)99; expected = UMI_STATUS_INVALID_STATE; }
    else if (!strcmp(name, "null")) { CANCEL_CHECK(UmiOrderCancel(NULL) == UMI_STATUS_INVALID_ARGUMENT); return 0; }
    else return 2;
    UmiOrder desired;
    memcpy(&desired, &order, sizeof(desired));
    if (expected == UMI_STATUS_OK && changes) { desired.status = UMI_ORDER_CANCELLED; ++desired.version; }
    CANCEL_CHECK(UmiOrderCancel(&order) == expected);
    CANCEL_CHECK(!memcmp(&order, &desired, sizeof(order)));
    if (expected == UMI_STATUS_OK) {
        CANCEL_CHECK(UmiOrderCancel(&order) == UMI_STATUS_OK);
        CANCEL_CHECK(!memcmp(&order, &desired, sizeof(order)));
    }
    return 0;
}
static int CancelBrokerCase(const char *name)
{
    UmiSimulatedBrokerStorage *storage = calloc(1U, sizeof(*storage));
    CANCEL_CHECK(storage != NULL);
    UmiBroker broker = {0};
    UmiOrderRequest request = test_order_request();
    request.environment = !strcmp(name, "broker-paper") ? UMI_TRADING_PAPER : UMI_TRADING_SIMULATION;
    CANCEL_CHECK(umi_simulated_broker_create(storage, &broker) == UMI_STATUS_OK);
    CANCEL_CHECK(broker.connect(broker.instance, request.environment) == UMI_STATUS_OK);
    CANCEL_CHECK(broker.submit_order(broker.instance, &request) == UMI_STATUS_OK);
    UmiOrder *order = &storage->orders.orders[0];
    order->version = UINT64_MAX - 1U;
    CANCEL_CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_OK);
    UmiOrder saved;
    memcpy(&saved, order, sizeof(saved));
    CANCEL_CHECK(saved.status == UMI_ORDER_CANCELLED && saved.version == UINT64_MAX);
    CANCEL_CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_OK);
    CANCEL_CHECK(!memcmp(&saved, order, sizeof(saved)));
    broker.destroy(broker.instance); free(storage); return 0;
}
static int CancelWorkspaceCase(const char *name)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    config.risk_limit = (UmiRiskLimit){1000.0, 1.0e12, 5000.0, 1.0e12};
    CANCEL_CHECK(umi_trading_workspace_create(&config, &workspace) == UMI_STATUS_OK);
    if (!strcmp(name, "missing")) {
        CANCEL_CHECK(umi_trading_workspace_cancel_selected_order(workspace) == UMI_STATUS_NOT_FOUND);
        umi_trading_workspace_destroy(workspace); return 0;
    }
    UmiInstrument instrument = test_instrument(); instrument.multiplier = 1.0;
    UmiRiskDecision decision;
    CANCEL_CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
    CANCEL_CHECK(umi_trading_workspace_set_health(workspace, 1, 0, 1) == UMI_STATUS_OK);
    CANCEL_CHECK(umi_trading_workspace_set_draft_quantity(workspace, 2.0) == UMI_STATUS_OK);
    CANCEL_CHECK(umi_trading_workspace_set_draft_prices(workspace, 100.0, 0.0) == UMI_STATUS_OK);
    CANCEL_CHECK(umi_trading_workspace_submit_order(workspace, 1000, &decision) == UMI_STATUS_OK);
    UmiOrder *order = &workspace->oms.orders.orders[0];
    if (!strcmp(name, "workspace-partial") || !strcmp(name, "filled-stability")) {
        UmiExecutionReport report = {0};
        strcpy(report.execution_id.value, "before-cancel");
        report.client_order_id = order->request.client_order_id;
        report.fill_quantity = !strcmp(name, "filled-stability") ? 2.0 : 0.5;
        report.fill_price = 100.0; report.event_time_ms = 1000;
        CANCEL_CHECK(umi_trading_workspace_record_execution(workspace, &report) == UMI_STATUS_OK);
    }
    UmiStatus expected = UMI_STATUS_OK;
    int changes = 1;
    if (!strcmp(name, "order-exhausted")) { order->version = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(name, "workspace-exhausted")) { workspace->revision = UINT64_MAX; expected = UMI_STATUS_CAPACITY_EXCEEDED; }
    else if (!strcmp(name, "both-final")) { workspace->revision = UINT64_MAX - 1U; order->version = UINT64_MAX - 1U; }
    else if (!strcmp(name, "cancelled-max")) {
        CANCEL_CHECK(umi_trading_workspace_cancel_selected_order(workspace) == UMI_STATUS_OK);
        workspace->revision = UINT64_MAX; order->version = UINT64_MAX; changes = 0;
    } else if (!strcmp(name, "filled-stability")) expected = UMI_STATUS_INVALID_STATE;
    else if (!strcmp(name, "filter-retire")) workspace->order_filter = UMI_TRADING_WORKSPACE_ORDERS_OPEN;
    else if (strcmp(name, "workspace-accepted") && strcmp(name, "workspace-partial") && strcmp(name, "workspace-replay")) {
        umi_trading_workspace_destroy(workspace); return 2;
    }
    /* Snapshot the complete owner on the heap: it contains bounded market
     * history, so a large automatic copy would obscure the tested ownership. */
    void *saved = malloc(sizeof(*workspace));
    CANCEL_CHECK(saved != NULL); memcpy(saved, workspace, sizeof(*workspace));
    UmiOrder oldOrder;
    memcpy(&oldOrder, order, sizeof(oldOrder));
    const uint64_t revision = workspace->revision;
    const size_t executions = workspace->executions.count, positions = workspace->positions.count;
    CANCEL_CHECK(umi_trading_workspace_cancel_selected_order(workspace) == expected);
    if (expected != UMI_STATUS_OK || !changes) {
        CANCEL_CHECK(!memcmp(saved, workspace, sizeof(*workspace)));
    } else {
        CANCEL_CHECK(order->status == UMI_ORDER_CANCELLED && order->version == oldOrder.version + 1U);
        CANCEL_CHECK(order->filled_quantity == oldOrder.filled_quantity && order->average_fill_price == oldOrder.average_fill_price);
        CANCEL_CHECK(workspace->revision == revision + 1U);
        CANCEL_CHECK(workspace->executions.count == executions && workspace->positions.count == positions);
        if (!strcmp(name, "workspace-partial")) {
            UmiPosition position;
            CANCEL_CHECK(umi_trading_workspace_position_at(workspace, 0U, &position) == UMI_STATUS_OK);
            CANCEL_CHECK(position.quantity == 0.5 && position.average_price == 100.0);
        }
        memcpy(saved, workspace, sizeof(*workspace));
        CANCEL_CHECK(umi_trading_workspace_cancel_selected_order(workspace) ==
            (!strcmp(name, "filter-retire") ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK));
        CANCEL_CHECK(!memcmp(saved, workspace, sizeof(*workspace)));
    }
    free(saved); umi_trading_workspace_destroy(workspace); return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!strcmp(argv[1], "broker-paper") || !strcmp(argv[1], "broker-simulation")) return CancelBrokerCase(argv[1]);
    if (!strncmp(argv[1], "workspace-", 10U) || !strcmp(argv[1], "order-exhausted") ||
        !strcmp(argv[1], "both-final") || !strcmp(argv[1], "cancelled-max") ||
        !strcmp(argv[1], "filled-stability") || !strcmp(argv[1], "filter-retire") || !strcmp(argv[1], "missing"))
        return CancelWorkspaceCase(argv[1]);
    return CancelValueCase(argv[1]);
}
