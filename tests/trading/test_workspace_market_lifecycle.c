/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_workspace_market_lifecycle.c
 *
 * PURPOSE:
 *   Verify market removal preserves surviving chart views and reused slots start with default navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include "test_trading_common.h"

#define CHECK(expression) do { \
    if (!(expression)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression); \
        return 1; \
    } \
} while (0)

static UmiInstrument named(const char *id)
{
    UmiInstrument instrument = test_instrument();
    (void)snprintf(instrument.instrument_id.value, sizeof(instrument.instrument_id.value), "%s", id);
    return instrument;
}

static int same_navigation(UmiChartNavigation a, UmiChartNavigation b)
{
    return a.visible_bars == b.visible_bars && a.anchor_ms == b.anchor_ms &&
           a.pinned == b.pinned && a.interval_ms == b.interval_ms;
}

static int select_and_check(UmiTradingWorkspace *workspace, const char *id,
                            UmiChartNavigation expected)
{
    UmiChartNavigation actual = {0};
    CHECK(umi_trading_workspace_select_instrument(workspace, id) == UMI_STATUS_OK);
    CHECK(UmiTradingWorkspaceGetChartNavigation(workspace, id, &actual) == UMI_STATUS_OK);
    CHECK(same_navigation(actual, expected));
    return 0;
}

static int removal_and_reuse(void)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    CHECK(umi_trading_workspace_create(&config, &workspace) == UMI_STATUS_OK);
    const UmiInstrument a = named("MARKET.A"), b = named("MARKET.B"), c = named("MARKET.C");
    const UmiChartNavigation nav_a = {5U, 1000, 1, 60000U};
    const UmiChartNavigation nav_b = {7U, 2000, 1, 300000U};
    const UmiChartNavigation nav_c = {9U, 3000, 1, 900000U};
    const UmiChartNavigation default_view = {0};
    CHECK(umi_trading_workspace_add_instrument(workspace, &a) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_add_instrument(workspace, &b) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_add_instrument(workspace, &c) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_select_instrument(workspace, "MARKET.A") == UMI_STATUS_OK);
    CHECK(UmiTradingWorkspaceSetChartNavigation(workspace, "MARKET.A", &nav_a) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_select_instrument(workspace, "MARKET.B") == UMI_STATUS_OK);
    CHECK(UmiTradingWorkspaceSetChartNavigation(workspace, "MARKET.B", &nav_b) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_select_instrument(workspace, "MARKET.C") == UMI_STATUS_OK);
    CHECK(UmiTradingWorkspaceSetChartNavigation(workspace, "MARKET.C", &nav_c) == UMI_STATUS_OK);
    /* Removing the middle row must move C's entire presentation state. */
    CHECK(umi_trading_workspace_remove_instrument(workspace, "MARKET.B") == UMI_STATUS_OK);
    CHECK(select_and_check(workspace, "MARKET.A", nav_a) == 0);
    CHECK(select_and_check(workspace, "MARKET.C", nav_c) == 0);
    CHECK(umi_trading_workspace_add_instrument(workspace, &b) == UMI_STATUS_OK);
    CHECK(select_and_check(workspace, "MARKET.B", default_view) == 0);
    CHECK(UmiTradingWorkspaceSetChartNavigation(workspace, "MARKET.B", &nav_b) == UMI_STATUS_OK);
    /* Removing the first row exercises a multi-element compaction. */
    CHECK(umi_trading_workspace_remove_instrument(workspace, "MARKET.A") == UMI_STATUS_OK);
    CHECK(select_and_check(workspace, "MARKET.C", nav_c) == 0);
    CHECK(select_and_check(workspace, "MARKET.B", nav_b) == 0);
    /* Removing the selected tail has no memmove, but must clear its state. */
    CHECK(umi_trading_workspace_remove_instrument(workspace, "MARKET.B") == UMI_STATUS_OK);
    CHECK(select_and_check(workspace, "MARKET.C", nav_c) == 0);
    CHECK(umi_trading_workspace_add_instrument(workspace, &a) == UMI_STATUS_OK);
    CHECK(select_and_check(workspace, "MARKET.A", default_view) == 0);
    CHECK(umi_trading_workspace_remove_instrument(workspace, "MARKET.A") == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_remove_instrument(workspace, "MARKET.C") == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_add_instrument(workspace, &c) == UMI_STATUS_OK);
    CHECK(select_and_check(workspace, "MARKET.C", default_view) == 0);
    umi_trading_workspace_destroy(workspace);
    return 0;
}

static int refused_queries_preserve_view(void)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    UmiInstrument instrument = named("MARKET.A");
    const UmiChartNavigation expected = {7U, 2000, 1, 300000U};
    CHECK(umi_trading_workspace_create(&config, &workspace) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
    CHECK(UmiTradingWorkspaceSetChartNavigation(workspace, "MARKET.A", &expected) == UMI_STATUS_OK);
    UmiTradingWorkspaceSnapshot before, after;
    CHECK(umi_trading_workspace_snapshot(workspace, &before) == UMI_STATUS_OK);
    char query[UMI_FINANCE_ID_CAPACITY];
    memset(query, 'q', sizeof(query));
    UmiChartNavigation output = expected;
    CHECK(UmiTradingWorkspaceGetChartNavigation(workspace, query, &output) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(same_navigation(output, expected));
    CHECK(UmiTradingWorkspaceSetChartNavigation(workspace, query, &expected) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(umi_trading_workspace_select_instrument(workspace, query) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_trading_workspace_remove_instrument(workspace, query) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_trading_workspace_set_market_state(workspace, query, UMI_MARKET_OPEN) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiTradingWorkspaceGetChartNavigation(workspace, "MARKET.B", &output) == UMI_STATUS_INVALID_STATE);
    CHECK(same_navigation(output, expected));
    CHECK(umi_trading_workspace_remove_instrument(workspace, "missing") == UMI_STATUS_NOT_FOUND);
    CHECK(umi_trading_workspace_snapshot(workspace, &after) == UMI_STATUS_OK);
    CHECK(before.revision == after.revision && before.market_count == after.market_count);
    CHECK(strcmp(before.selected_instrument_id, after.selected_instrument_id) == 0);
    CHECK(UmiTradingWorkspaceGetChartNavigation(workspace, "MARKET.A", &output) == UMI_STATUS_OK);
    CHECK(same_navigation(output, expected));
    umi_trading_workspace_destroy(workspace);
    return 0;
}

int main(void)
{
    CHECK(removal_and_reuse() == 0);
    CHECK(refused_queries_preserve_view() == 0);
    return 0;
}
