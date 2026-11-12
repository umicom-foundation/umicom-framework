/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_snapshot_revision.c
 *
 * PURPOSE:
 *   Verify that snapshots expose the trading owner revision independently of positions and nested observations.
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
#include "umicom/trading/session_report.h"
/* Use the real owner for final-revision and independent child observations. */
#include "../../src/trading/workspace.c"
#define SNAP_CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

static int SnapshotRead(UmiTradingWorkspace *workspace, UmiTradingWorkspaceSnapshot *snapshot)
{
    SNAP_CHECK(umi_trading_workspace_snapshot(workspace, snapshot) == UMI_STATUS_OK);
    SNAP_CHECK(snapshot->revision == workspace->revision);
    UmiTradingSessionReport *report = NULL;
    UmiTradingSessionSummary summary;
    SNAP_CHECK(UmiTradingSessionReportCapture(workspace, NULL, &report) == UMI_STATUS_OK);
    SNAP_CHECK(UmiTradingSessionReportSummary(report, &summary) == UMI_STATUS_OK);
    SNAP_CHECK(summary.revision == snapshot->revision);
    UmiTradingSessionReportDestroy(report);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    UmiTradingWorkspace *workspace = NULL;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    config.risk_limit = (UmiRiskLimit){1000.0, 1.0e12, 5000.0, 1.0e12};
    UmiTradingWorkspaceSnapshot before, after;
    SNAP_CHECK(umi_trading_workspace_create(&config, &workspace) == UMI_STATUS_OK);
    if (!strcmp(name, "zero")) workspace->revision = 0U;
    else if (!strcmp(name, "maximum")) workspace->revision = UINT64_MAX;
    else if (!strcmp(name, "near-maximum")) workspace->revision = UINT64_MAX - 1U;
    else if (!strcmp(name, "tape-only")) {
        SNAP_CHECK(SnapshotRead(workspace, &before) == 0);
        SNAP_CHECK(umi_trading_trade_tape_set_provider_ready(workspace->trade_tape, 1) == UMI_STATUS_OK);
        SNAP_CHECK(SnapshotRead(workspace, &after) == 0);
        SNAP_CHECK(after.revision == before.revision);
        SNAP_CHECK(after.trade_tape.revision > before.trade_tape.revision);
        umi_trading_workspace_destroy(workspace); return 0;
    } else if (!strcmp(name, "partial") || !strcmp(name, "cancelled") ||
               !strcmp(name, "flatten") || !strcmp(name, "report-agreement")) {
        UmiInstrument instrument = test_instrument(); instrument.multiplier = 1.0;
        UmiRiskDecision decision;
        SNAP_CHECK(umi_trading_workspace_add_instrument(workspace, &instrument) == UMI_STATUS_OK);
        SNAP_CHECK(umi_trading_workspace_set_health(workspace, 1, 0, 1) == UMI_STATUS_OK);
        SNAP_CHECK(umi_trading_workspace_set_draft_quantity(workspace, 2.0) == UMI_STATUS_OK);
        SNAP_CHECK(umi_trading_workspace_set_draft_prices(workspace, 100.0, 0.0) == UMI_STATUS_OK);
        SNAP_CHECK(umi_trading_workspace_submit_order(workspace, 1000, &decision) == UMI_STATUS_OK);
        UmiExecutionReport fill = {0};
        strcpy(fill.execution_id.value, "open-position");
        fill.client_order_id = workspace->oms.orders.orders[0].request.client_order_id;
        fill.fill_quantity = 1.0; fill.fill_price = 100.0; fill.event_time_ms = 1000;
        SNAP_CHECK(SnapshotRead(workspace, &before) == 0);
        SNAP_CHECK(umi_trading_workspace_record_execution(workspace, &fill) == UMI_STATUS_OK);
        SNAP_CHECK(SnapshotRead(workspace, &after) == 0);
        SNAP_CHECK(after.revision == before.revision + 1U);
        if (!strcmp(name, "cancelled")) {
            before = after;
            SNAP_CHECK(umi_trading_workspace_cancel_selected_order(workspace) == UMI_STATUS_OK);
            SNAP_CHECK(SnapshotRead(workspace, &after) == 0);
            SNAP_CHECK(after.revision == before.revision + 1U);
        } else if (!strcmp(name, "flatten")) {
            SNAP_CHECK(umi_trading_workspace_set_draft_side(workspace, UMI_SIDE_SELL) == UMI_STATUS_OK);
            SNAP_CHECK(umi_trading_workspace_set_draft_quantity(workspace, 1.0) == UMI_STATUS_OK);
            SNAP_CHECK(umi_trading_workspace_submit_order(workspace, 1001, &decision) == UMI_STATUS_OK);
            fill.client_order_id = workspace->oms.orders.orders[1].request.client_order_id;
            strcpy(fill.execution_id.value, "close-position"); fill.event_time_ms = 1001;
            SNAP_CHECK(SnapshotRead(workspace, &before) == 0);
            SNAP_CHECK(umi_trading_workspace_record_execution(workspace, &fill) == UMI_STATUS_OK);
            SNAP_CHECK(SnapshotRead(workspace, &after) == 0);
            SNAP_CHECK(after.revision == before.revision + 1U);
            SNAP_CHECK(workspace->positions.positions[0].quantity == 0.0);
        }
    } else if (strcmp(name, "empty") && strcmp(name, "repeat")) {
        umi_trading_workspace_destroy(workspace); return 2;
    }
    SNAP_CHECK(SnapshotRead(workspace, &before) == 0);
    SNAP_CHECK(SnapshotRead(workspace, &after) == 0);
    SNAP_CHECK(after.revision == before.revision);
    SNAP_CHECK(after.charts.revision == before.charts.revision);
    SNAP_CHECK(after.trade_tape.revision == before.trade_tape.revision);
    umi_trading_workspace_destroy(workspace);
    return 0;
}
