/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_day_orders.c
 *
 * PURPOSE:
 *   Verify venue-scoped simulated DAY cancellation and atomic admission.
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
/* Use the real workspace implementation to exercise otherwise unreachable
 * revision and store boundaries without adding a mutable product API. */
#include "../../src/trading/workspace.c"

#define CHECK_DAY(condition) do { \
    if (!(condition)) { fprintf(stderr, "%d: %s\n", __LINE__, #condition); result = 1; goto done; } \
} while (0)

static UmiStatus seed_order(UmiTradingWorkspace *workspace, const char *id,
    const char *venue, UmiTimeInForce policy, UmiOrderStatus status, UmiTradingEnvironment environment)
{
    UmiOrder order = {.request = test_order_request(), .status = status, .version = 1U};
    strcpy(order.request.client_order_id.value, id);
    strcpy(order.request.instrument.venue, venue);
    order.request.environment = environment;
    order.request.tif = policy;
    if (status == UMI_ORDER_PARTIALLY_FILLED) {
        order.filled_quantity = order.request.quantity / 2.0;
        order.average_fill_price = 101.0;
    } else if (status == UMI_ORDER_FILLED) {
        order.filled_quantity = order.request.quantity;
        order.average_fill_price = 101.0;
    }
    return umi_order_store_add(&workspace->oms.orders, &order);
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    UmiTradingWorkspace *workspace = NULL;
    UmiOrderStore *saved = calloc(1U, sizeof *saved);
    UmiStatus expected = UMI_STATUS_OK;
    const char *venue = "LOCAL";
    size_t cancelled = 99U;
    int result = 0;
    CHECK_DAY(saved != NULL);
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    CHECK_DAY(umi_trading_workspace_create(&config, &workspace) == UMI_STATUS_OK);
    workspace->environment = !strcmp(test, "paper") ? UMI_TRADING_PAPER : UMI_TRADING_SIMULATION;
    CHECK_DAY(seed_order(workspace, "accepted", "LOCAL", UMI_TIF_DAY, UMI_ORDER_ACCEPTED,
        workspace->environment) == UMI_STATUS_OK);
    CHECK_DAY(seed_order(workspace, "partial", "LOCAL", UMI_TIF_DAY, UMI_ORDER_PARTIALLY_FILLED,
        workspace->environment) == UMI_STATUS_OK);
    CHECK_DAY(seed_order(workspace, "gtc", "LOCAL", UMI_TIF_GTC, UMI_ORDER_ACCEPTED,
        workspace->environment) == UMI_STATUS_OK);
    CHECK_DAY(seed_order(workspace, "other-venue", "LOCAL-X", UMI_TIF_DAY, UMI_ORDER_ACCEPTED,
        workspace->environment) == UMI_STATUS_OK);
    CHECK_DAY(seed_order(workspace, "filled", "LOCAL", UMI_TIF_DAY, UMI_ORDER_FILLED,
        workspace->environment) == UMI_STATUS_OK);
    CHECK_DAY(seed_order(workspace, "other-mode", "LOCAL", UMI_TIF_DAY, UMI_ORDER_ACCEPTED,
        workspace->environment == UMI_TRADING_PAPER ? UMI_TRADING_SIMULATION : UMI_TRADING_PAPER) == UMI_STATUS_OK);
    CHECK_DAY(umi_trading_workspace_select_order(workspace, "accepted") == UMI_STATUS_OK);

    if (!strcmp(test, "order-exhausted")) {
        workspace->oms.orders.orders[1].version = UINT64_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(test, "workspace-exhausted")) {
        workspace->revision = UINT64_MAX;
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (!strcmp(test, "live")) {
        workspace->environment = UMI_TRADING_LIVE;
        expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (!strcmp(test, "stale")) {
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(test, "corrupt")) {
        memset(workspace->oms.orders.orders[1].request.instrument.venue, 'a', UMI_TRADING_VENUE_CAPACITY);
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(test, "duplicate-id")) {
        workspace->oms.orders.orders[1].request.client_order_id =
            workspace->oms.orders.orders[0].request.client_order_id;
        expected = UMI_STATUS_INVALID_STATE;
    } else if (!strcmp(test, "empty-venue")) {
        venue = "";
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (!strcmp(test, "no-match")) {
        workspace->revision = UINT64_MAX;
        venue = "UNKNOWN";
    } else if (strcmp(test, "simulation") && strcmp(test, "paper")) {
        result = 2;
        goto done;
    }

    memcpy(saved, &workspace->oms.orders, sizeof *saved);
    const uint64_t revision = workspace->revision;
    const uint64_t expected_revision = revision - (!strcmp(test, "stale") ? 1U : 0U);
    CHECK_DAY(UmiTradingWorkspaceCloseDayOrders(workspace, venue, expected_revision, &cancelled) == expected);
    if (expected != UMI_STATUS_OK || !strcmp(test, "no-match")) {
        CHECK_DAY(cancelled == (expected == UMI_STATUS_OK ? 0U : 99U));
        CHECK_DAY(workspace->revision == revision);
        CHECK_DAY(!memcmp(saved, &workspace->oms.orders, sizeof *saved));
        CHECK_DAY(!strcmp(workspace->selected_order_id, "accepted"));
        goto done;
    }
    CHECK_DAY(cancelled == 2U && workspace->revision == revision + 1U);
    for (size_t i = 0U; i < 2U; ++i) {
        saved->orders[i].status = UMI_ORDER_CANCELLED;
        ++saved->orders[i].version;
    }
    CHECK_DAY(!memcmp(saved, &workspace->oms.orders, sizeof *saved));
    CHECK_DAY(UmiTradingWorkspaceCloseDayOrders(workspace, venue, revision, &cancelled) == UMI_STATUS_INVALID_STATE);
    CHECK_DAY(cancelled == 2U);
    CHECK_DAY(UmiTradingWorkspaceCloseDayOrders(workspace, venue, revision + 1U, &cancelled) == UMI_STATUS_OK);
    CHECK_DAY(cancelled == 0U && workspace->revision == revision + 1U);
    CHECK_DAY(!memcmp(saved, &workspace->oms.orders, sizeof *saved));
done:
    umi_trading_workspace_destroy(workspace);
    free(saved);
    return result;
}
