/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/operational_state/test_trading_drafts.c
 * PURPOSE: Rejected numeric input must preserve the reviewed trading draft.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test); return EXIT_FAILURE; } } while (0)
int main(void)
{
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingWorkspaceSnapshot before, after;
    UmiInstrument instrument = test_instrument();
    UmiRiskDecision risk;
    double bad[] = {NAN, INFINITY, -INFINITY, -1.0};
    CHECK(umi_trading_workspace_create(NULL, &workspace) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_quantity(workspace, 1.0) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_prices(workspace, 100.0, 0.0) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_preview_order(workspace, &risk) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_snapshot(workspace, &before) == UMI_STATUS_OK);
    for (size_t i = 0U; i < sizeof(bad)/sizeof(bad[0]); ++i) {
        CHECK(umi_trading_workspace_set_draft_quantity(workspace, bad[i]) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_trading_workspace_set_draft_prices(workspace, bad[i], 0.0) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_trading_workspace_set_draft_prices(workspace, 100.0, bad[i]) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_trading_workspace_snapshot(workspace, &after) == UMI_STATUS_OK);
        CHECK(memcmp(&before, &after, sizeof(before)) == 0);
    }
    UmiBar bar = {0};
    bar.instrument = instrument; bar.open = 100.0; bar.high = 110.0;
    bar.low = 90.0; bar.close = 105.0; bar.volume = 10.0;
    bar.start_time_ms = 100; bar.end_time_ms = 200;
    CHECK(umi_trading_workspace_update_bar(workspace, &bar, NAN) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_trading_workspace_update_bar(workspace, &bar, INFINITY) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_trading_workspace_selected_bar_count(workspace) == 0U);
    memset(&bar.instrument.instrument_id, 'X', sizeof(bar.instrument.instrument_id));
    CHECK(umi_trading_workspace_update_bar(workspace, &bar, 100.0) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_trading_workspace_snapshot(workspace, &after) == UMI_STATUS_OK);
    CHECK(memcmp(&before, &after, sizeof(before)) == 0);
    CHECK(umi_trading_workspace_set_draft_quantity(workspace, DBL_MAX) == UMI_STATUS_OK);
    CHECK(umi_trading_workspace_set_draft_prices(workspace, 0.0, 0.0) == UMI_STATUS_OK);
    /* Draft editing permits finite values; the independent request/risk checks
     * still decide whether an order is complete and acceptable. */
    CHECK(umi_trading_workspace_preview_order(workspace, &risk) != UMI_STATUS_OK);
    umi_trading_workspace_destroy(workspace);
    return EXIT_SUCCESS;
}
