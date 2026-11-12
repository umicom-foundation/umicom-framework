/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_workspace_order_lookup.c
 *
 * PURPOSE:
 *   Verify workspace order selection uses bounded shared identity lookup.
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
#include "test_trading_common.h"

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
} while (0)

int main(void)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    config.risk_limit = (UmiRiskLimit){1000.0, 100000000.0, 5000.0, 1000000.0};
    UmiInstrument instrument = test_instrument();
    UmiRiskDecision decision;
    UmiTradingWorkspaceSnapshot before, after;
    CHECK(umi_trading_workspace_create(&config, &workspace) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_health(workspace, 1, 0, 1) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_quantity(workspace, 1.0) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_prices(workspace, 100.0, 0.0) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_submit_order(workspace, 1000, &decision) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_snapshot(workspace, &before) == UMI_STATUS_OK);
    char selected[sizeof(before.selected_order_id)];
    memcpy(selected, before.selected_order_id, sizeof(selected));
    CHECK(umi_trading_workspace_select_order(workspace, selected) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_snapshot(workspace, &before) == UMI_STATUS_OK);
    char unterminated[sizeof(((UmiFinancialId *)0)->value)];
    memset(unterminated, 'q', sizeof(unterminated));
    CHECK(umi_trading_workspace_select_order(workspace, unterminated) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_trading_workspace_select_order(workspace, "missing") == UMI_STATUS_NOT_FOUND);
    CHECK(umi_trading_workspace_select_order(workspace, "") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_trading_workspace_snapshot(workspace, &after) == UMI_STATUS_OK);
    CHECK(before.revision == after.revision);
    CHECK(strcmp(before.selected_order_id, after.selected_order_id) == 0);
    umi_trading_workspace_destroy(workspace);
    return 0;
}
